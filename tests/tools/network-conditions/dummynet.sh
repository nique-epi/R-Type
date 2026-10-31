#!/bin/sh
# macOS: makes every packet that leaves the loopback interface cross the
# network of a scenario of the game, with dummynet, so a server and its clients
# on one machine play over it without any option of their own.
#
#   sudo tests/tools/network-conditions/dummynet.sh apply --network bad
#   sudo tests/tools/network-conditions/dummynet.sh clear
#
# dnctl configures a dummynet pipe, and a pf rule sends through it every packet
# leaving lo0. A packet between two programs of this machine leaves lo0 once,
# so a ping, which crosses it twice, gains the lag. dummynet delays and loses
# packets but never varies the delay, copies or reorders them: the jitter, the
# duplicates and the reordering of a scenario are left out, with a warning.
set -eu

# shellcheck source-path=SCRIPTDIR source=conditions.sh
. "$(dirname "$0")/conditions.sh"

TOOL=dummynet
INTERFACE=lo0

# The dummynet pipe of the script. Pipes are shared by the whole system: this
# number, the default port of the server, is only unlikely to be taken.
PIPE=4242

# The pf anchor of the rule. /etc/pf.conf evaluates the dummynet rules of every
# anchor under com.apple (dummynet-anchor "com.apple/*"), so the rule needs no
# change to the main ruleset.
ANCHOR=com.apple/rtype-network-conditions

# Where apply keeps the token of `pfctl -E` until clear releases it with
# `pfctl -X`, so pf goes back to the state it was in.
TOKEN_FILE=/var/run/rtype-network-conditions.pf-token

# Warns about the values dummynet cannot apply.
warn_about_what_dummynet_lacks() {
  lacking=''
  [ "$jitter" -eq 0 ] || lacking="$lacking, up to $jitter ms more"
  [ "$duplicates" -eq 0 ] || lacking="$lacking, $duplicates % duplicated"
  [ "$reorder" -eq 0 ] || lacking="$lacking, $reorder % reordered"
  if [ -n "$lacking" ]; then
    printf '%s: dummynet cannot apply%s: left out\n' "${0##*/}" \
      "${lacking#,}" >&2
  fi
}

# Prints the loss as dummynet takes it: a share from 0 to 1, such as 0.10.
packet_loss_rate() {
  printf '%d.%02d\n' "$((loss / 100))" "$((loss % 100))"
}

# Loads the pf rule $1 into the anchor, or prints how when only printing.
load_rule() {
  if [ "$printing" = yes ]; then
    printf "echo '%s' | pfctl -a %s -f -\n" "$1" "$ANCHOR"
  else
    printf '%s\n' "$1" | pfctl -a "$ANCHOR" -f -
  fi
}

# Enables pf, once: an apply that follows another keeps the first token.
enable_pf() {
  if [ "$printing" = yes ]; then
    printf 'pfctl -E\n'
  elif [ ! -s "$TOKEN_FILE" ]; then
    enabled=$(pfctl -E 2>&1)
    token=$(printf '%s\n' "$enabled" | sed -n 's/^Token : //p')
    [ -n "$token" ] || fail "pfctl -E gave no token: $enabled"
    printf '%s\n' "$token" >"$TOKEN_FILE"
  fi
}

# Configures the pipe with half the lag, rounded up to the millisecond, and
# the loss, then sends every packet leaving the interface through it.
apply_conditions() {
  warn_about_what_dummynet_lacks
  run dnctl pipe "$PIPE" config delay "$(((lag + 1) / 2))" \
    plr "$(packet_loss_rate)"
  load_rule "dummynet out quick on $INTERFACE from any to any pipe $PIPE"
  enable_pf
}

# Empties the anchor, deletes the pipe and releases the reference apply took
# on pf.
remove_conditions() {
  pfctl -a "$ANCHOR" -F all
  if dnctl pipe show "$PIPE" | grep -q .; then
    dnctl pipe delete "$PIPE"
  fi
  if [ -s "$TOKEN_FILE" ]; then
    pfctl -X "$(cat "$TOKEN_FILE")"
    rm -f "$TOKEN_FILE"
  fi
}

run_command "$@"
