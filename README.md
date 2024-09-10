# tcp_proxy
Secure and Minimal tcp proxy. Runs as non-privileged user and can therefore only proxy to unprivileged ports (>=1024).

To activate traffic logging set VAR_FINAL_COMMAND='socat1 -d -d -d -d TCP-LISTEN:${VAR_LISTEN_PORT:-$VAR_PORT},fork,reuseaddr TCP:$VAR_HOST:$VAR_PORT'

## Environment variables
### pre-set runtime variables
* VAR_PORT="8080" (port on recieving host)
* VAR_FINAL_COMMAND='socat1 TCP-LISTEN:${VAR_LISTEN_PORT:-$VAR_PORT},fork,reuseaddr TCP:$VAR_HOST:$VAR_PORT'
* VAR_LINUX_USER="proxy"

### Runtime variables
* VAR_HOST (recieving host)
* VAR_LISTEN_PORT (the port this container listens to internally. Optional, same as VAR_PORT if not set)

## Capabilities
Can drop all but CHOWN, SETPCAP, SETGID and SETUID.
