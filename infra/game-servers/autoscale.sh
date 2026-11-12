#!/bin/sh
# One pass of the autoscaler of the game servers: reads on the master how
# full the running servers of the pool are, and starts the next one when
# SCALE_OUT_PERCENT of the games they can run are running. It does nothing
# when the master does not answer or when a server that runs is not listed
# online yet, and it never stops a server, which would end its games.
# rtype-autoscaler.timer runs it every 15 seconds.
# Usage: autoscale.sh [<settings file>]
set -eu

scripts=$(cd -- "$(dirname -- "$0")" && pwd)

# shellcheck source=infra/game-servers/autoscaler.conf.example
. "${1:-/etc/rtype-autoscaler.conf}"

main() {
  if ! listing=$("$scripts/listed-servers.sh" "$MASTER_URL" "$PUBLIC_ADDRESS"); then
    echo 'the master did not answer: nothing done'
    return 0
  fi

  games=0
  most_games=0
  next=
  for port in $PORTS; do
    if ! systemctl is-active --quiet "rtype-server@$port"; then
      [ -n "$next" ] || next=$port
      continue
    fi
    # The games of the server and the games it can run, once the master
    # lists it as taking players.
    room=$(printf '%s\n' "$listing" |
      awk -v port="$port" '$1 == port && ($2 == "online" || $2 == "degraded") { print $3, $4 }')
    if [ -z "$room" ]; then
      echo "server $port runs but is not listed online yet: nothing done"
      return 0
    fi
    games=$((games + ${room% *}))
    most_games=$((most_games + ${room#* }))
  done

  if [ "$most_games" -eq 0 ]; then
    echo 'no server of the pool runs: nothing done'
    return 0
  fi
  # A game holds at most the players of its seats, so the share of the seats
  # taken never passes the share of the games: the games alone decide.
  if [ $((games * 100)) -lt $((SCALE_OUT_PERCENT * most_games)) ]; then
    return 0
  fi
  if [ -z "$next" ]; then
    echo "pool exhausted: $games of $most_games games run"
    return 0
  fi
  echo "starting server $next: $games of $most_games games run"
  systemctl start "rtype-server@$next"
}

main
