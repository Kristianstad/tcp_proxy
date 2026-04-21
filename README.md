# tcp_proxy
Secure and Minimal tcp proxy. Runs as non-privileged user and can therefore only proxy to unprivileged ports (>=1024).

To activate traffic logging modify VAR_FINAL_COMMAND with additional -d flags.

## Environment variables
### pre-set runtime variables
* VAR_PORT="8080" (port on recieving host)
* VAR_FINAL_COMMAND='socat1 -d TCP-LISTEN:${VAR_LISTEN_PORT:-$VAR_PORT},fork,reuseaddr,backlog=512,so-reuseport,keepalive,keepidle=60,keepintvl=10,keepcnt=6,nodelay TCP:$VAR_HOST:$VAR_PORT,connect-timeout=14,keepalive,keepidle=60,keepintvl=10,keepcnt=6,nodelay'
* VAR_LINUX_USER="proxy"

### Runtime variables
* VAR_HOST (recieving host)
* VAR_LISTEN_PORT (the port this container listens to internally. Optional, same as VAR_PORT if not set)

## Capabilities
Can drop all but CHOWN, SETPCAP, SETGID and SETUID, but might also need NET_BIND_SERVICE, NET_RAW AND NET_ADMIN.
