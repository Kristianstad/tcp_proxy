#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/* Utfor Postgres-protokollets SSL-forhandling och sedan en riktig
   TLS-handskakning ovanpa den redan uppkopplade TCP-anslutningen 's'.
   Returnerar 0 om handskakningen lyckas, annars 1. */
static int do_ssl_check(int s) {
    /* SSLRequest = Int32(8) langd + Int32(80877103) specialkod.
       Servern (pgbouncer) svarar med en enda byte: 'S' (kor SSL) eller
       'N' (vill inte). Detta maste ske innan nagra TLS-bytes skickas. */
    unsigned char sslreq[8];
    uint32_t len  = htonl(8);
    uint32_t code = htonl(80877103);
    memcpy(sslreq, &len, 4);
    memcpy(sslreq + 4, &code, 4);

    if (write(s, sslreq, sizeof(sslreq)) != (ssize_t)sizeof(sslreq)) {
        fprintf(stderr, "healthcheck: kunde inte skicka SSLRequest\n");
        return 1;
    }

    unsigned char resp;
    ssize_t n = read(s, &resp, 1);
    if (n != 1 || resp != 'S') {
        fprintf(stderr, "healthcheck: servern gick inte med pa SSL (%s)\n",
                n == 1 ? "fel byte" : "inget svar/stangd anslutning");
        return 1;
    }

    OPENSSL_init_ssl(0, NULL);
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        fprintf(stderr, "healthcheck: SSL_CTX_new misslyckades\n");
        return 1;
    }
    /* Vi vill bara veta att sjalva handskakningen gar igenom, inte om vi
       litar pa certifikatet - darfor ingen verifiering har. */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

    int ok = 1;
    SSL *ssl = SSL_new(ctx);
    if (ssl) {
        SSL_set_fd(ssl, s);
        if (SSL_connect(ssl) == 1) {
            ok = 0;
            /* Stang snyggt (close_notify) sa pgbouncer inte loggar en
               "unexpected eof"-varning om oss vid varje enskild koll. */
            SSL_shutdown(ssl);
        } else {
            fprintf(stderr, "healthcheck: TLS-handskakningen misslyckades\n");
            ERR_print_errors_fp(stderr);
        }
        SSL_free(ssl);
    } else {
        fprintf(stderr, "healthcheck: SSL_new misslyckades\n");
    }
    SSL_CTX_free(ctx);
    return ok;
}

int main(int argc, char **argv) {
    const char *host = NULL;
    int port = 6432;
    int use_tls = 0;
    const char *p;

    /* Plocka ut --tls var den an star i argumentlistan; ovriga argument
       forblir positionella och i ordning precis som tidigare, sa
       befintliga anrop (utan --tls) fortsatter fungera oforandrat. */
    const char *pos[2] = {NULL, NULL};
    int npos = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tls") == 0) {
            use_tls = 1;
        } else if (npos < 2) {
            pos[npos++] = argv[i];
        }
    }
    if (npos > 0) host = pos[0];
    if (npos > 1) port = atoi(pos[1]);

    if ((p = getenv("VAR_param_listen_port")) != NULL)
        port = atoi(p);
    if (port <= 0 || port > 65535)
        return 1;

    in_addr_t addr = htonl(INADDR_LOOPBACK);
    if (host != NULL) {
        in_addr_t parsed = inet_addr(host);
        if (parsed == (in_addr_t)-1)
            return 1;
        addr = parsed;
    }

    signal(SIGALRM, SIG_DFL);
    /* Utan denna dodar en motpart som stanger anslutningen mitt i en
       skrivning (exakt det produktionsfelet handlar om) processen med
       SIGPIPE istallet for att lata write()/SSL_connect() returnera ett
       vanligt fel. */
    signal(SIGPIPE, SIG_IGN);
    alarm(4);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return 1;
    struct sockaddr_in a = {0};
    a.sin_family      = AF_INET;
    a.sin_port        = htons((unsigned short)port);
    a.sin_addr.s_addr = addr;

    if (connect(s, (struct sockaddr *)&a, sizeof(a)) != 0) {
        close(s);
        return 1;
    }

    int result = use_tls ? do_ssl_check(s) : 0;
    close(s);
    return result;
}
