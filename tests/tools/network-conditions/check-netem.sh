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
# waiting a second at most. Sets sent to how many ping says it sent, replies to
# how many came back, fastest, average and spread (the mdev of ping) to their
# round trips in milliseconds, and in_order to 1 when the replies came back in
# the order of their sequence numbers, 0 otherwise.
ping_loopback() {
  requested=$1
  output=$(ping -n -c "$1" -i "$2" -W 1 127.0.0.1 2>&1 || true)
  sent=$(printf '%s\n' "$output" |
    sed -n 's/^\([0-9]*\) packets transmitted.*/\1/p')
  replies=$(printf '%s\n' "$output" | grep -c ' icmp_seq=' || true)
  read -r fastest average spread <<EOF
$(printf '%s\n' "$output" |
    sed -n 's|^rtt min/avg/max/mdev = \([0-9.]*\)/\([0-9.]*\)/[0-9.]*/\([0-9.]*\) ms.*|\1 \2 \3|p')
EOF
  if printf '%s\n' "$output" | sed -n 's/.* icmp_seq=\([0-9]*\) .*/\1/p' |
    sort -n -c; then
    in_order=1
  else
    in_order=0
  fi
}

# Checks the condition $2, an awk expression over replies, fastest, average,
# spread and in_order, once ping sent every echo request asked: a ping that
# could not send them measured nothing. $1 says what is checked.
expect() {
  if [ "${sent:-0}" -eq "$requested" ] &&
    awk -v replies="$replies" -v fastest="${fastest:-0}" \
      -v average="${average:-0}" -v spread="${spread:-0}" \
      -v in_order="$in_order" "BEGIN { exit !($2) }"; then
    printf 'ok: %s\n' "$1"
  else
    printf 'FAILED: %s: %s of %s sent, %s replies, fastest %s ms, average %s ms, spread %s ms, in order %s\n' \
      "$1" "${sent:-none}" "$requested" "$replies" "${fastest:-none}" \
      "${average:-none}" "${spread:-none}" "$in_order"
    printf '%s\n' "$output" | tail -n 3
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
# 10 ms apart would pass each other if netem reordered them. Over five runs in
# an ubuntu:24.04 container, the round trips spread by 17.4 to 23.3 ms with the
# jitter and by 1.7 to 2.3 ms with the same delay and no jitter, so a spread
# under 8 ms means the jitter was not applied.
sh "$here/netem.sh" apply --jitter 200
ping_loopback 100 0.01
expect 'the jitter spreads the round trips and keeps the packets in order' \
  'replies == 100 && in_order == 1 && spread >= 8'

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
