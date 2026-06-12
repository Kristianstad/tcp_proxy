#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
int main(int argc, char **argv) {
    const char *host = NULL;
    int port = 0;
    const char *p;
    if (argc > 2) {
        host = argv[1];
        port = atoi(argv[2]);
    } else if (argc > 1) {
        host = argv[1];
    }
    if (port <= 0 && (p = getenv("VAR_LISTEN_PORT")) != NULL)
        port = atoi(p);
    if (port <= 0 && (p = getenv("VAR_PORT")) != NULL)
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
    alarm(3);
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return 1;
    struct sockaddr_in a = {0};
    a.sin_family      = AF_INET;
    a.sin_port        = htons((unsigned short)port);
    a.sin_addr.s_addr = addr;
    int r = connect(s, (struct sockaddr *)&a, sizeof(a));
    close(s);
    return r == 0 ? 0 : 1;
}
