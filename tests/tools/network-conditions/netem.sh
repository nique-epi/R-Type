#!/bin/sh
# Linux: makes every packet that leaves the loopback interface cross the
# network of a scenario of the game, with netem, so a server and its clients on
# one machine play over it without any option of their own.
#
#   sudo tests/tools/network-conditions/netem.sh apply --network bad
#   sudo tests/tools/network-conditions/netem.sh clear
#
# A packet between two programs of this machine leaves the loopback interface
# once, so a ping, which crosses it twice, gains the lag and the jitter, as with
# the options of the programs. Everything on the interface is slowed down, not
# only the game.
set -eu

# shellcheck source-path=SCRIPTDIR source=conditions.sh
. "$(dirname "$0")/conditions.sh"

TOOL=netem
INTERFACE=lo

# The rate netem gives the interface: far above what a game sends, so a
# datagram of 1200 bytes waits under a microsecond more. With a rate, netem
# counts the delay of a packet from the last packet queued (netem_enqueue() in
# net/sched/sch_netem.c), so the jitter never makes a packet leave before the
# one queued before it, as in the simulator of the game.
LINK_RATE=10gbit

# Replaces the queue of the interface with netem. Each packet waits half the
# lag, then up to half the jitter more: netem draws its wait uniformly within
# the jitter around the delay, so the delay is half the lag plus a quarter of
# the jitter, and the netem jitter a quarter of it, in microseconds.
apply_conditions() {
  delay_us=$((lag * 500 + jitter * 250))
  if [ "$reorder" -gt 0 ] && [ "$delay_us" -eq 0 ]; then
    fail 'netem reorders only delayed packets: give --lag or --jitter too'
  fi
  set -- delay "${delay_us}us" "$((jitter * 250))us" \
    loss "${loss}%" duplicate "${duplicates}%"
  if [ "$reorder" -gt 0 ]; then
    set -- "$@" reorder "${reorder}%"
  fi
  run tc qdisc replace dev "$INTERFACE" root netem "$@" rate "$LINK_RATE"
}

# Gives the interface its own queue back, if netem replaced it.
remove_conditions() {
  if tc qdisc show dev "$INTERFACE" | grep -q '^qdisc netem [0-9a-f]*: root'; then
    tc qdisc del dev "$INTERFACE" root
  fi
}

run_command "$@"
