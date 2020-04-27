# sam-tcp_proxy
Secure and Minimal tcp proxy.

## Environment variables
### pre-set runtime variables
* VAR_PORT="8080"
* VAR_FINAL_COMMAND="/usr/local/bin/postgres --config_file=\"\$VAR_CONFIG_FILE\""
* VAR_LINUX_USER="proxy"

### Runtime variables
* VAR_HOST
* VAR_LISTEN_PORT (optional)

## Capabilities
Can drop all but SETPCAP, SETGID and SETUID.
