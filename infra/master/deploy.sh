#!/bin/sh
# Installs the server package of a release on the master instance and
# restarts the master on it; goes back to the previous release when the new
# one does not answer. Caddy answers HTTPS for DOMAIN in front of it, with a
# certificate of Let's Encrypt it renews by itself. The workflow runs it
# through SSM Run Command, after the lines VERSION=<x.y.z> and
# DOMAIN=<name of the master>.
set -eu

repository=nique-epi/R-Type
port=8080
releases=/opt/rtype/releases
current=/opt/rtype/current

# The version comes from a tag: it is never used before it is checked.
require_version() {
  : "${VERSION:?VERSION is not set}"
  case $VERSION in
    '' | *[!0-9.]*)
      echo "invalid version: $VERSION" >&2
      exit 1
      ;;
  esac
}

# The domain comes from a variable of the repository: it is never written in
# the configuration of Caddy before it is checked.
require_domain() {
  : "${DOMAIN:?DOMAIN is not set}"
  if ! printf '%s' "$DOMAIN" | grep -Eq '^([a-z0-9]([a-z0-9-]*[a-z0-9])?\.)+[a-z]{2,}$'; then
    echo "invalid domain: $DOMAIN" >&2
    exit 1
  fi
}

# Installs Caddy from Ubuntu once, then has it answer HTTPS for DOMAIN and
# forward every request to the master on the machine. Caddy writes the
# address it saw in X-Forwarded-For and drops the one a client sent, which is
# the address the master reads.
install_proxy() {
  if ! command -v caddy > /dev/null; then
    apt-get update -q
    DEBIAN_FRONTEND=noninteractive apt-get install -y -q caddy
  fi
  cat > /etc/caddy/Caddyfile <<CADDY
$DOMAIN {
	reverse_proxy 127.0.0.1:$port
}
CADDY
  caddy validate --config /etc/caddy/Caddyfile --adapter caddyfile
  systemctl enable caddy
  systemctl reload-or-restart caddy
}

# Downloads and unpacks a release next to the others. A missing release stops
# here, before anything changed on the machine.
unpack_release() {
  name=$1
  if [ -d "$releases/$name" ]; then
    return 0
  fi
  url=https://github.com/$repository/releases/download/v$VERSION/$name.tar.gz
  work=$(mktemp -d)
  trap 'rm -rf "$work"' EXIT
  curl --fail --silent --show-error --location "$url" --output "$work/server.tar.gz"
  tar -xzf "$work/server.tar.gz" -C "$work"
  mv "$work/$name" "$releases/$name"
}

install_service() {
  cat > /etc/systemd/system/rtype-master.service <<UNIT
[Unit]
Description=R-Type master
After=network-online.target
Wants=network-online.target

[Service]
User=rtype
Group=rtype
WorkingDirectory=/var/lib/rtype
ExecStart=$current/bin/r-type_server --mode master -p $port --database /var/lib/rtype/r-type_master.db --no-log-file
Restart=on-failure
RestartSec=5
NoNewPrivileges=true
ProtectSystem=strict
ReadWritePaths=/var/lib/rtype
PrivateTmp=true

[Install]
WantedBy=multi-user.target
UNIT
  systemctl daemon-reload
  systemctl enable rtype-master
}

# Runs the master on the release folder it is given.
run_release() {
  ln -sfn "$1" "$current"
  systemctl restart rtype-master
}

# Succeeds as soon as the master answers on the machine itself, so a failure
# here comes from the master and not from the network in front of it.
wait_until_healthy() {
  for _ in $(seq 1 20); do
    if curl --fail --silent "http://127.0.0.1:$port/api/health" > /dev/null; then
      return 0
    fi
    sleep 1
  done
  return 1
}

# Shows why the new release did not answer, then runs the previous one again.
roll_back() {
  journalctl -u rtype-master --no-pager -n 30 >&2 || true
  if [ -n "$1" ]; then
    run_release "$1"
    echo "went back to $1" >&2
  fi
}

# Keeps the release in use and the previous one.
prune_releases() {
  for release in "$releases"/*/; do
    release=${release%/}
    if [ "$release" != "$1" ] && [ "$release" != "$2" ]; then
      rm -rf "$release"
    fi
  done
}

main() {
  require_version
  require_domain
  name=R-Type-server-$VERSION-Linux
  previous=$(readlink "$current" 2>/dev/null || true)

  unpack_release "$name"
  install_service
  run_release "$releases/$name"

  if ! wait_until_healthy; then
    echo "the master of $VERSION does not answer" >&2
    roll_back "$previous"
    exit 1
  fi

  prune_releases "$releases/$name" "$previous"
  install_proxy
  echo "the master runs $VERSION behind https://$DOMAIN"
}

main
