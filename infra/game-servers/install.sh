#!/bin/sh
# Installs the release VERSION for the pool of game servers of this machine,
# one server per port of PORTS, with the service of each one, then starts
# them all. It stops before changing anything when a server of the pool has
# no server.cfg with its key of the master, in /var/lib/rtype-servers/<port>/.
# Run it as root; run again, it installs another release and restarts the
# servers, which ends their games.
# Usage: VERSION=<x.y.z> PORTS="<port> ..." infra/game-servers/install.sh
set -eu

scripts=$(cd -- "$(dirname -- "$0")" && pwd)
releases=/opt/rtype-servers/releases
servers=/var/lib/rtype-servers

# shellcheck source=infra/release.sh
. "$scripts/../release.sh"

require_pool() {
  : "${PORTS:?PORTS is not set}"
  for port in $PORTS; do
    if ! grep -Eq '^set master-key "?rtk_' "$servers/$port/server.cfg" 2> /dev/null; then
      echo "$servers/$port/server.cfg has no key of the master (set master-key)" >&2
      exit 1
    fi
  done
}

install_tools() {
  if command -v curl > /dev/null; then
    return 0
  fi
  apt-get update -q
  DEBIAN_FRONTEND=noninteractive apt-get install -y -q curl
}

prepare_folders() {
  if ! id -u rtype > /dev/null 2>&1; then
    useradd --system --home-dir "$servers" --shell /usr/sbin/nologin rtype
  fi
  install -d "$releases"
  install -d -o rtype -g rtype "$servers"
}

# Every server runs from its own copy, so the unpacked release is only kept
# the time to copy it.
install_release() {
  name=R-Type-server-$VERSION-Linux
  unpack_release "$releases" "$name"
  for port in $PORTS; do
    chown -R rtype:rtype "$servers/$port"
    chmod 600 "$servers/$port/server.cfg"
    copy_release_for "$releases/$name" "$servers/$port"
  done
  rm -rf "$releases"
}

# One service per server, named after its port.
install_service() {
  write_server_unit rtype-server@ 'R-Type game server on port %i' "$servers/%i" \
    "$servers/%i/release/bin/r-type_server -p %i --database $servers/%i/leaderboard.db --no-log-file"
  systemctl daemon-reload
}

# A server that already runs moves to the new release; the others start.
start_pool() {
  systemctl try-restart 'rtype-server@*.service'
  for port in $PORTS; do
    systemctl enable --now "rtype-server@$port"
  done
}

main() {
  require_version
  require_pool
  install_tools
  prepare_folders
  install_release
  install_service
  start_pool
  echo "the pool runs $VERSION: the servers of the ports $PORTS are started"
}

main
