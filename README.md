# sam-tcp_proxy
Secure and Minimal tcp proxy. Runs as non-privileged user and can therefore only proxy to unprivileged ports (>=1024).

## Environment variables
### pre-set runtime variables
* VAR_PORT="8080"
* VAR_FINAL_COMMAND='socat TCP-LISTEN:${VAR_LISTEN_PORT:-$VAR_PORT},fork,reuseaddr TCP:$VAR_HOST:$VAR_PORT'
* VAR_LINUX_USER="proxy"

### Runtime variables
* VAR_HOST
* VAR_LISTEN_PORT (optional)

## Capabilities
Can drop all but CHOWN, SETPCAP, SETGID and SETUID.
