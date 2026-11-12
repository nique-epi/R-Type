#!/bin/sh
#
# Checks what one pass of the autoscaler of the game servers decides
# (infra/game-servers/autoscale.sh), against a master and a systemctl that
# this script stands in for: a curl that prints the listing of the case, and
# a systemctl that says which servers run and writes down what it is asked.
#
# Usage: sh tests/tools/infra/check-autoscaler.sh
#
# The CI runs it with the checks of infra/. It stops at the first case whose
# pass asked systemctl something else than expected.

set -eu

autoscaler=$(cd -- "$(dirname -- "$0")/../../../infra/game-servers" && pwd)/autoscale.sh
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

mkdir "$work/bin"

# Prints the listing of the case, or fails as curl does when the master is
# down.
cat > "$work/bin/curl" <<'FAKE'
#!/bin/sh
if [ ! -e "$WORK/listing.json" ]; then
  exit 7
fi
cat "$WORK/listing.json"
FAKE

# Answers is-active from the ports of the file running, and writes every
# start and stop it is asked to the file asked.
cat > "$work/bin/systemctl" <<'FAKE'
#!/bin/sh
case $1 in
  is-active)
    grep -Fxq "${3#rtype-server@}" "$WORK/running"
    ;;
  start | stop)
    echo "$1 ${2#rtype-server@}" >> "$WORK/asked"
    ;;
esac
FAKE
chmod +x "$work/bin/curl" "$work/bin/systemctl"

cat > "$work/autoscaler.conf" <<'CONF'
MASTER_URL=http://master.test
PUBLIC_ADDRESS=203.0.113.10
PORTS="4242 4243 4244"
SCALE_OUT_PERCENT=70
CONF

# Prints one server of a listing: its address, port, status and games.
server() {
  printf '{"address":"%s","udpPort":%s,"status":"%s","gameCount":%s,"playerCount":%s,"maximumGames":8,"maximumPlayersPerGame":4}' \
    "$1" "$2" "$3" "$4" "$4"
}

# Sets the servers the master lists, from the servers given.
master_lists() {
  listing=
  for listed in "$@"; do
    listing=${listing:+$listing,}$listed
  done
  printf '{"servers":[%s]}' "$listing" > "$work/listing.json"
}

# Sets the ports whose server runs.
running() {
  printf '%s\n' "$@" > "$work/running"
}

# Runs one pass, then fails unless systemctl was asked exactly $2; $1 names
# the case.
expect() {
  : > "$work/asked"
  WORK=$work PATH=$work/bin:$PATH sh "$autoscaler" "$work/autoscaler.conf" > "$work/output"
  asked=$(tr '\n' ';' < "$work/asked")
  if [ "$asked" != "$2" ]; then
    echo "FAILED: $1: asked '$asked', not '$2'" >&2
    cat "$work/output" >&2
    exit 1
  fi
  echo "ok: $1"
}

# 70 % of 8 games is 5.6: 5 games leave the pool as it is, 6 start a server.
running 4242
master_lists "$(server 203.0.113.10 4242 online 5)"
expect 'five games of eight start nothing' ''

master_lists "$(server 203.0.113.10 4242 online 6)"
expect 'six games of eight start the next server' 'start 4243;'

# 70 % of 16 games is 11.2: the games of every running server count.
running 4242 4243
master_lists "$(server 203.0.113.10 4242 online 8)" "$(server 203.0.113.10 4243 online 3)"
expect 'eleven games of sixteen start nothing' ''

master_lists "$(server 203.0.113.10 4242 online 8)" "$(server 203.0.113.10 4243 online 4)"
expect 'twelve games of sixteen start the third server' 'start 4244;'

master_lists "$(server 203.0.113.10 4242 degraded 8)" "$(server 203.0.113.10 4243 online 4)"
expect 'a degraded server still counts' 'start 4244;'

# A server that runs and is not listed yet: its room is unknown, so the pass
# waits instead of starting one more.
master_lists "$(server 203.0.113.10 4242 online 8)"
expect 'a server not listed yet starts nothing' ''

# Its last figures would fill the pool if they were counted.
master_lists "$(server 203.0.113.10 4242 online 8)" "$(server 203.0.113.10 4243 offline 8)"
expect 'a server listed offline starts nothing' ''

master_lists "$(server 203.0.113.10 4242 online 8)" "$(server 198.51.100.7 4243 online 8)"
expect 'a server of another address is not of the pool' ''

running 4242 4243 4244
master_lists "$(server 203.0.113.10 4242 online 8)" "$(server 203.0.113.10 4243 online 8)" \
  "$(server 203.0.113.10 4244 online 8)"
expect 'a full pool starts nothing' ''

running 4242
rm "$work/listing.json"
expect 'a master that does not answer starts nothing' ''

printf 'not json' > "$work/listing.json"
expect 'an answer that is not a listing starts nothing' ''

: > "$work/running"
master_lists "$(server 203.0.113.10 4242 online 8)"
expect 'a pool with no running server starts nothing' ''

# The autoscaler only starts servers: a stop would end games.
running 4242 4243
master_lists "$(server 203.0.113.10 4242 online 0)" "$(server 203.0.113.10 4243 online 0)"
expect 'two empty servers stop nothing' ''

echo 'every case of the autoscaler passed'
