#!/bin/sh
# Installs the release VERSION for the pool of game servers of this machine,
# with the service of each server and the autoscaler, then starts the first
# server of the pool. It reads the pool in /etc/rtype-autoscaler.conf (see
# autoscaler.conf.example) and stops before changing anything when a server
# of the pool has no server.cfg with its key of the master. Run it as root;
# run again, it installs another release and restarts the running servers,
# which ends their games.
# Usage: VERSION=<x.y.z> infra/game-servers/install.sh
set -eu

scripts=$(cd -- "$(dirname -- "$0")" && pwd)
configuration=/etc/rtype-autoscaler.conf
releases=/opt/rtype-servers/releases
servers=/var/lib/rtype-servers

# shellcheck source=infra/release.sh
. "$scripts/../release.sh"

require_pool() {
  if [ ! -r "$configuration" ]; then
    echo "$configuration is missing: copy autoscaler.conf.example there" >&2
    exit 1
  fi
  # shellcheck source=infra/game-servers/autoscaler.conf.example
  . "$configuration"
  for port in $PORTS; do
    if ! grep -Eq '^set master-key "?rtk_' "$servers/$port/server.cfg" 2> /dev/null; then
      echo "$servers/$port/server.cfg has no key of the master (set master-key)" >&2
      exit 1
    fi
  done
}

install_tools() {
  if command -v curl > /dev/null && command -v jq > /dev/null; then
    return 0
  fi
  apt-get update -q
  DEBIAN_FRONTEND=noninteractive apt-get install -y -q curl jq
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

# One service per server, named after its port. Only the first server of the
# pool is enabled: the autoscaler starts the others.
install_services() {
  write_server_unit rtype-server@ 'R-Type game server on port %i' "$servers/%i" \
    "$servers/%i/release/bin/r-type_server -p %i --database $servers/%i/leaderboard.db --no-log-file"
  install -m 755 "$scripts/autoscale.sh" "$scripts/listed-servers.sh" /opt/rtype-servers/
  cat > /etc/systemd/system/rtype-autoscaler.service <<UNIT
[Unit]
Description=One pass of the autoscaler of the R-Type game servers

[Service]
Type=oneshot
ExecStart=/opt/rtype-servers/autoscale.sh $configuration
UNIT
  write_timer rtype-autoscaler 'Runs the autoscaler of the R-Type game servers every 15 seconds' 30 15
  systemctl daemon-reload
}

start_pool() {
  systemctl try-restart 'rtype-server@*.service'
  systemctl enable --now "rtype-server@${PORTS%% *}"
  systemctl enable --now rtype-autoscaler.timer
}

main() {
  require_version
  require_pool
  install_tools
  prepare_folders
  install_release
  install_services
  start_pool
  echo "the pool runs $VERSION: server ${PORTS%% *} started, the others wait for the autoscaler"
}

main
