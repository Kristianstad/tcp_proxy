# tcp_proxy
Secure and Minimal tcp proxy. Runs as non-privileged user and can therefore only proxy to unprivileged ports (>=1024).

To activate traffic logging set VAR_FINAL_COMMAND='socat1 -d -d -d -d TCP-LISTEN:${VAR_LISTEN_PORT:-$VAR_PORT},fork,reuseaddr TCP:$VAR_HOST:$VAR_PORT'

## Environment variables
### pre-set runtime variables
* VAR_PORT="8080" (port on recieving host)
* VAR_FINAL_COMMAND='socat1 -d -t 20 TCP-LISTEN:${VAR_LISTEN_PORT:-$VAR_PORT},fork,reuseaddr,backlog=512,keepalive,keepidle=7200,keepintvl=75,keepcnt=9 TCP:$VAR_HOST:$VAR_PORT,connect-timeout=14,keepalive,keepidle=7200,keepintvl=75,keepcnt=9'
* VAR_LINUX_USER="proxy"

### Runtime variables
* VAR_HOST (recieving host)
* VAR_LISTEN_PORT (the port this container listens to internally. Optional, same as VAR_PORT if not set)

## Capabilities
Can drop all but CHOWN, SETPCAP, SETGID and SETUID, but might also need NET_BIND_SERVICE, NET_RAW AND NET_ADMIN.
