#!/bin/sh
# Checks the commands netem.sh and dummynet.sh print for given options, so the
# way they turn the values of a network into netem and dummynet settings is
# tested on any system, without root. Registered in CTest as
# network_condition_commands.
#
# Each expected text is worked out from the rule the scripts follow, not copied
# from their output: a ping crosses the loopback interface twice, so each packet
# waits half the lag and up to half the jitter more.
set -eu

here=$(dirname "$0")
status=0

# Checks that `print`, given to the script $2 (netem or dummynet) with the
# arguments after it, writes the text $3, standard error included, and exits
# as the last line of it says; $1 says what is checked.
expect() {
  description=$1
  script=$2
  expected=$3
  shift 3
  if output=$(sh "$here/$script.sh" print "$@" 2>&1); then
    code=0
  else
    code=$?
  fi
  actual=$(printf '%s\nexit %s\n' "$output" "$code")
  if [ "$actual" = "$expected" ]; then
    printf 'ok: %s\n' "$description"
  else
    printf 'FAILED: %s\n--- expected\n%s\n--- written\n%s\n' \
      "$description" "$expected" "$actual"
    status=1
  fi
}

# netem: half the lag plus a quarter of the jitter, within a quarter of it.
expect 'netem gives bad 150 to 175 ms on each packet' netem \
  'tc qdisc replace dev lo root netem delay 162500us 12500us loss 10% duplicate 2% reorder 5% rate 10gbit
exit 0' --network bad

expect 'netem leaves the reordering out of 4g, which has none' netem \
  'tc qdisc replace dev lo root netem delay 35000us 5000us loss 1% duplicate 0% rate 10gbit
exit 0' --network 4g

expect 'netem takes a value given after the network instead of its own' netem \
  'tc qdisc replace dev lo root netem delay 35000us 5000us loss 3% duplicate 0% rate 10gbit
exit 0' --network 4g --loss 3

expect 'netem gives a jitter alone 0 to 20 ms on each packet' netem \
  'tc qdisc replace dev lo root netem delay 10000us 10000us loss 0% duplicate 0% rate 10gbit
exit 0' --jitter 40

expect 'netem refuses to reorder packets it does not delay' netem \
  'netem.sh: netem reorders only delayed packets: give --lag or --jitter too
exit 1' --reorder 5

# The bounds and the errors are those of the programs.
expect 'the longest lag is taken' netem \
  'tc qdisc replace dev lo root netem delay 5000000us 0us loss 0% duplicate 0% rate 10gbit
exit 0' --lag 10000

expect 'a lag longer than the longest is refused' netem \
  'netem.sh: Invalid value "10001" after the option --lag
exit 1' --lag 10001

expect 'a full loss is taken' netem \
  'tc qdisc replace dev lo root netem delay 0us 0us loss 100% duplicate 0% rate 10gbit
exit 0' --loss 100

expect 'a share above a hundred is refused' netem \
  'netem.sh: Invalid value "101" after the option --duplicates
exit 1' --duplicates 101

expect 'a value written with leading zeros is read in decimal' netem \
  'tc qdisc replace dev lo root netem delay 0us 0us loss 10% duplicate 0% rate 10gbit
exit 0' --loss 010

expect 'an unknown network is refused' netem \
  'netem.sh: Invalid value "5g" after the option --network
exit 1' --network 5g

expect 'an option without its value is refused' netem \
  'netem.sh: Missing value after the option --lag
exit 1' --lag

expect 'a perfect network is refused' netem \
  'netem.sh: nothing to simulate: give --network or a value, or use clear
exit 1' --lag 0

# dummynet: half the lag, rounded up to the millisecond, and the loss as a
# share; what it lacks is left out with a warning.
expect 'dummynet gives bad 150 ms and a tenth lost, without the rest' dummynet \
  "dummynet.sh: dummynet cannot apply up to 50 ms more, 2 % duplicated, 5 % reordered: left out
dnctl pipe 4242 config delay 150 plr 0.10
echo 'dummynet out quick on lo0 from any to any pipe 4242' | pfctl -a com.apple/rtype-network-conditions -f -
pfctl -E
exit 0" --network bad

expect 'dummynet rounds half of an odd lag up' dummynet \
  "dnctl pipe 4242 config delay 13 plr 0.00
echo 'dummynet out quick on lo0 from any to any pipe 4242' | pfctl -a com.apple/rtype-network-conditions -f -
pfctl -E
exit 0" --lag 25

expect 'dummynet loses every packet at a full loss' dummynet \
  "dnctl pipe 4242 config delay 0 plr 1.00
echo 'dummynet out quick on lo0 from any to any pipe 4242' | pfctl -a com.apple/rtype-network-conditions -f -
pfctl -E
exit 0" --loss 100

exit "$status"
