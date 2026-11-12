#!/bin/sh
# Installs the server package of a release on the master instance and
# restarts the master on it; goes back to the previous release when the new
# one does not answer. Caddy answers HTTPS for DOMAIN in front of it, with a
# certificate of Let's Encrypt it renews by itself. A timer restarts a master
# that stopped answering, and the test game server of the machine, when it
# runs, moves to the release too. The workflow runs it through SSM Run
# Command, after the lines VERSION=<x.y.z> and DOMAIN=<name of the master>
# and the functions of infra/release.sh.
set -eu

port=8080
releases=/opt/rtype/releases
current=/opt/rtype/current
test_server=/var/lib/rtype/test-server

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

install_service() {
  write_server_unit rtype-master 'R-Type master' /var/lib/rtype \
    "$current/bin/r-type_server --mode master -p $port --database /var/lib/rtype/r-type_master.db --no-log-file"
  systemctl daemon-reload
  systemctl enable rtype-master
}

# Restart=on-failure starts a master that stopped again, not one that still
# runs and no longer answers. Every 30 seconds this timer asks its health
# route, and restarts it when two requests 5 seconds apart both fail.
install_health_check() {
  health="curl --fail --silent --max-time 5 --output /dev/null http://127.0.0.1:$port/api/health"
  cat > /etc/systemd/system/rtype-master-health.service <<UNIT
[Unit]
Description=Restarts the R-Type master when it stopped answering

[Service]
Type=oneshot
ExecStart=/bin/sh -c '$health || { sleep 5; $health; } || systemctl restart rtype-master'
UNIT
  write_timer rtype-master-health 'Checks the R-Type master every 30 seconds' 60 30
  systemctl daemon-reload
  systemctl enable --now rtype-master-health.timer
}

# The game server the team tests on between two LANs. It reads server.cfg in
# its folder, which a person writes once with its key of the master, and runs
# from its own copy of the release (see copy_release_for). Nothing here
# enables it: the workflow lan-close does, and lan-open stops it. A running
# one is restarted on the new release, which ends its games.
install_test_server() {
  install -d -o rtype -g rtype "$test_server"
  copy_release_for "$1" "$test_server"
  write_server_unit rtype-test-server 'R-Type test game server' "$test_server" \
    "$test_server/release/bin/r-type_server --database $test_server/leaderboard.db --no-log-file"
  systemctl daemon-reload
  systemctl try-restart rtype-test-server
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

  unpack_release "$releases" "$name"
  install_service
  run_release "$releases/$name"

  if ! wait_until_healthy; then
    echo "the master of $VERSION does not answer" >&2
    roll_back "$previous"
    exit 1
  fi

  prune_releases "$releases/$name" "$previous"
  install_health_check
  install_test_server "$releases/$name"
  install_proxy
  echo "the master runs $VERSION behind https://$DOMAIN"
}

main
