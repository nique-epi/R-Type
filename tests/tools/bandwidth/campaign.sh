#!/bin/sh
# Measures what one client receives and sends per second in each scenario of
# the bandwidth campaign (docs/src/content/docs/measurements.md): a server and
# BOT_COUNT load-test bots on this machine, from the same seed, <runs> runs of
# <seconds> seconds per scenario. Prints one Markdown row per scenario, each
# figure the median over every bot of every run.
#
# Usage: sh tests/tools/bandwidth/campaign.sh <r-type_server> <load_test_bots>
#        [<runs> [<seconds>]]

set -eu

readonly PORT=4310
readonly BOT_COUNT=4
readonly SEED=1
readonly DEFAULT_RUNS=3
readonly DEFAULT_SECONDS=60
readonly IP_AND_UDP_HEADER_BYTES=28

server_pid=

usage() {
  echo 'usage: sh campaign.sh <r-type_server> <load_test_bots> [<runs> [<seconds>]]' >&2
  exit 2
}

# Writes the script name and $1 to standard error, then stops the script.
fail() {
  printf '%s: %s\n' "${0##*/}" "$1" >&2
  exit 1
}

# Sets number to the whole number $1, or exits through usage() unless it is
# above 0. Leading zeros are dropped, so the shell never reads it as octal.
whole_count() {
  case $1 in
    '' | *[!0-9]*) usage ;;
  esac
  number=${1#"${1%%[!0]*}"}
  if [ -z "$number" ]; then
    usage
  fi
}

# Starts one game server, with no supervisor, so the options given reach the
# games themselves; from the work folder, so no server.cfg of the caller
# applies; and keeps its process id in server_pid.
start_server() {
  (cd "$work" && exec "$server" --mode server -p "$PORT" --seed "$SEED" \
    --database "$work/results.db" --no-log-file --no-log-stderr "$@") &
  server_pid=$!
}

# Stops the server and fails unless it stopped cleanly.
stop_server() {
  kill "$server_pid"
  stopped=0
  wait "$server_pid" || stopped=$?
  server_pid=
  if [ "$stopped" -ne 0 ]; then
    fail 'the server did not stop cleanly'
  fi
}

# Stops the server the script leaves running when it ends early, so its port
# is free for the next campaign.
stop_leftover_server() {
  if [ -n "$server_pid" ]; then
    kill "$server_pid" 2> /dev/null || true
    wait "$server_pid" 2> /dev/null || true
  fi
}

# Runs the bots once against a server started with the options given, then
# appends to $figures one line per bot: the entities per state, the bytes
# received per second once decoded and as the network carried them, the
# datagrams received per second, the bytes of the network with the headers of
# IP and UDP, then the bytes and the datagrams sent per second.
run_once() {
  start_server "$@"
  report="$work/$name-$run.txt"
  if ! "$bots" -p "$PORT" --bots "$BOT_COUNT" --seconds "$seconds" \
    --seed "$SEED" > "$report" 2>&1; then
    fail "the bots failed; their report is $report"
  fi
  stop_server
  sed -n 's/^bot .* \([0-9.]*\) entities per state; received \([0-9.]*\) B\/s decoded and \([0-9.]*\) B\/s on the network in \([0-9.]*\) datagrams\/s, sent \([0-9.]*\) B\/s in \([0-9.]*\) datagrams\/s$/\1 \2 \3 \4 \5 \6/p' "$report" |
    awk -v headers="$IP_AND_UDP_HEADER_BYTES" \
      '{ printf "%s %s %s %s %.1f %s %s\n", $1, $2, $3, $4, $3 + (headers * $4), $5, $6 }' \
      >> "$figures"
}

# Prints the median of column $2 of the file $1.
median() {
  cut -d ' ' -f "$2" "$1" | sort -n | awk '
    { values[NR] = $1 }
    END {
      if (NR % 2 == 1) { printf "%.1f", values[(NR + 1) / 2] }
      else { printf "%.1f", (values[NR / 2] + values[(NR / 2) + 1]) / 2 }
    }'
}

# Measures the scenario named $1, its server started with the options after
# it, and prints its row.
scenario() {
  name=$1
  shift
  figures="$work/$name.txt"
  : > "$figures"
  run=1
  while [ "$run" -le "$runs" ]; do
    run_once "$@"
    run=$((run + 1))
  done
  if [ "$(awk 'END { print NR }' "$figures")" -ne $((runs * BOT_COUNT)) ]; then
    fail "a report of $name has a line the script cannot read"
  fi
  printf '| %s | %s |' "$name" "${*:-none}"
  for column in 1 2 3 4 5 6 7; do
    printf ' %s |' "$(median "$figures" "$column")"
  done
  echo
}

trap stop_leftover_server EXIT
trap 'exit 129' HUP
trap 'exit 130' INT
trap 'exit 143' TERM

if [ "$#" -lt 2 ] || [ "$#" -gt 4 ]; then
  usage
fi
server="$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
bots=$2
whole_count "${3:-$DEFAULT_RUNS}"
runs=$number
whole_count "${4:-$DEFAULT_SECONDS}"
seconds=$number
work=$(mktemp -d "${TMPDIR:-/tmp}/rtype-bandwidth.XXXXXX")

echo "One client of $BOT_COUNT bots, seed $SEED, $runs runs of $seconds s per scenario; medians. Reports in $work."
echo
echo '| Scenario | Server options | Entities per state | Received, decoded (B/s) | Received, on the network (B/s) | Datagrams received (/s) | Received with IP and UDP (B/s) | Sent (B/s) | Datagrams sent (/s) |'
echo '|---|---|---|---|---|---|---|---|---|'
scenario full-60 --state-rate 60 --set state-encoding=full
scenario full-20 --set state-encoding=full
scenario delta-20
scenario full-20-bad --set state-encoding=full --network bad
scenario delta-20-bad --network bad
