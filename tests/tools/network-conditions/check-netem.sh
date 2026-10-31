#!/bin/sh
# Applies network conditions to the loopback interface of this Linux machine
# with netem.sh, measures with ping what a packet meets, then removes them.
# It slows down everything on the interface while it runs: the CI job
# network-condition-scripts runs it, as root, on a runner of its own.
#
#   sudo sh tests/tools/network-conditions/check-netem.sh
set -eu

here=$(dirname "$0")
status=0
trap 'sh "$here/netem.sh" clear > /dev/null' EXIT

# Sends $1 echo requests to the loopback address, $2 seconds apart, each
# waiting a second at most. Sets replies to how many came back, fastest and
# average to their round trips in milliseconds, and in_order to 1 when the
# replies came back in the order of their sequence numbers, 0 otherwise.
ping_loopback() {
  output=$(ping -n -c "$1" -i "$2" -W 1 127.0.0.1 || true)
  replies=$(printf '%s\n' "$output" | grep -c ' icmp_seq=' || true)
  round_trips=$(printf '%s\n' "$output" |
    sed -n 's|^rtt min/avg/max/mdev = \([0-9.]*\)/\([0-9.]*\)/.*|\1 \2|p')
  fastest=${round_trips% *}
  average=${round_trips#* }
  if printf '%s\n' "$output" | sed -n 's/.* icmp_seq=\([0-9]*\) .*/\1/p' |
    sort -n -c; then
    in_order=1
  else
    in_order=0
  fi
}

# Checks the condition $2, an awk expression over replies, fastest, average
# and in_order; $1 says what is checked.
expect() {
  if awk -v replies="$replies" -v fastest="${fastest:-0}" \
    -v average="${average:-0}" -v in_order="$in_order" \
    "BEGIN { exit !($2) }"; then
    printf 'ok: %s\n' "$1"
  else
    printf 'FAILED: %s: %s replies, fastest %s ms, average %s ms, in order %s\n' \
      "$1" "$replies" "${fastest:-none}" "${average:-none}" "$in_order"
    status=1
  fi
}

# Given 4g without its loss, ping +60 ms and up to 20 ms more, each packet
# waits 30 to 40 ms.
sh "$here/netem.sh" apply --network 4g --loss 0
ping_loopback 20 0.2
expect '4g adds at least 60 ms to each round trip, under 80 ms on average' \
  'replies == 20 && fastest >= 60 && average < 80'

# Given a jitter of 200 ms, each packet waits 0 to 100 ms: echo requests sent
# 10 ms apart would pass each other if netem reordered them.
sh "$here/netem.sh" apply --jitter 200
ping_loopback 100 0.01
expect 'the jitter keeps the packets in order' 'replies == 100 && in_order == 1'

# Given every packet lost, no echo request comes back.
sh "$here/netem.sh" apply --loss 100
ping_loopback 3 0.2
expect 'a full loss lets no packet through' 'replies == 0'

# Given the conditions removed, twice, the loopback interface answers at once.
sh "$here/netem.sh" clear
sh "$here/netem.sh" clear
ping_loopback 5 0.2
expect 'clear gives the loopback interface its speed back' \
  'replies == 5 && average < 10'

exit "$status"
