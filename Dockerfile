# =========================================================================
# Init
# =========================================================================
# ARGs (can be passed to Build/Final) <BEGIN>
ARG SaM_REPO=${SaM_REPO:-ghcr.io/kristianstad/secure_and_minimal}
ARG ALPINE_VERSION=${ALPINE_VERSION:-3.23}
ARG APP_VERSION=${APP_VERSION:-1.8.0.3}
ARG IMAGETYPE="application"
ARG BUILDDEPS="openssl-dev"
ARG BUILDCMDS=\
"   gcc -O2 -o /tmp/healthcheck /tmp/healthcheck.c -lssl -lcrypto "\
"&& strip --strip-all /tmp/healthcheck "\
"&& chmod ugo+rx-w /tmp/healthcheck "\
"&& cp -a /tmp/healthcheck /finalfs/usr/local/bin/"
ARG RUNDEPS="socat"
ARG REMOVEFILES="/usr/bin/procan /usr/bin/filan /usr/bin/socat"
ARG STARTUPEXECUTABLES="/usr/bin/socat1"
# ARGs (can be passed to Build/Final) </END>

# Generic template (don't edit) <BEGIN>
FROM ${CONTENTIMAGE1:-scratch} AS content1
FROM ${CONTENTIMAGE2:-scratch} AS content2
FROM ${CONTENTIMAGE3:-scratch} AS content3
FROM ${CONTENTIMAGE4:-scratch} AS content4
FROM ${CONTENTIMAGE5:-scratch} AS content5
FROM ${BASEIMAGE:-$SaM_REPO:base-${ALPINE_VERSION}} AS base
FROM ${INITIMAGE:-scratch} AS init
# Generic template (don't edit) </END>

# =========================================================================
# Build
# =========================================================================
# Generic template (don't edit) <BEGIN>
FROM ${BUILDIMAGE:-$SaM_REPO:build-${ALPINE_VERSION}} AS build
FROM ${BASEIMAGE:-$SaM_REPO:base-${ALPINE_VERSION}} AS final
COPY --from=build /finalfs /
# Generic template (don't edit) </END>
COPY ./healthcheck.c /tmp/

# =========================================================================
# Final
# =========================================================================
# Re-declare ARGs
ARG ALPINE_VERSION
ARG APP_VERSION

ENV VAR_LINUX_USER="proxy" \
    VAR_PORT="8080" \
    VAR_KEEP_CAPS="cap_net_bind_service,cap_net_admin,cap_net_raw" \
    VAR_FINAL_COMMAND='socat1 -d TCP-LISTEN:${VAR_LISTEN_PORT:-$VAR_PORT},fork,reuseaddr,backlog=512,so-reuseport,keepalive,keepidle=60,keepintvl=10,keepcnt=6,nodelay TCP:$VAR_HOST:$VAR_PORT,connect-timeout=14,keepalive,keepidle=60,keepintvl=10,keepcnt=6,nodelay'

# Generic template (don't edit) <BEGIN>
USER starter
ONBUILD USER root
# Generic template (don't edit) </END>

LABEL org.opencontainers.image.version="${APP_VERSION}" \
      org.opencontainers.image.title="tcp_proxy" \
      org.opencontainers.image.description="TCP Proxy (socat) ${APP_VERSION} based on secure_and_minimal ${ALPINE_VERSION}"
