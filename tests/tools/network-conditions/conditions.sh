# The network conditions shared by netem.sh, dummynet.sh and their checks: the
# scenarios of the network simulator of the game, the reading of its options,
# and the commands apply, print and clear. Sourced, never run on its own.
# POSIX sh.

# The scenarios of `--network`, one per line: the name, the milliseconds the
# ping gains and may vary by, then the percent of the datagrams lost,
# duplicated and reordered. They are the scenarios of NETWORK_SCENARIOS
# (src/network/Transport/Conditions/NetworkConditionsConstants.hpp), which
# check-scenarios.sh compares with the help of the server.
SCENARIOS='4g 60 20 1 0 0
wifi-busy 30 60 3 1 2
bad 300 50 10 2 5'

# The largest lag or jitter, in milliseconds, and the largest share, in
# percent, the options take: MAXIMUM_SIMULATED_DELAY and MAXIMUM_PERCENT.
MAXIMUM_DELAY_MS=10000
MAXIMUM_PERCENT=100

# Writes the script name and $1 to standard error, then stops the script.
fail() {
  printf '%s: %s\n' "${0##*/}" "$1" >&2
  exit 1
}

# Prints the names of the scenarios, one per line.
scenario_names() {
  printf '%s\n' "$SCENARIOS" | cut -d ' ' -f 1
}

# Sets the five values to those of the scenario named $1.
use_scenario() {
  while read -r scenario lag jitter loss duplicates reorder; do
    [ "$scenario" != "$1" ] || return 0
  done <<EOF
$SCENARIOS
EOF
  fail "Invalid value \"$1\" after the option --network"
}

# Sets number to the whole number $2, given after the option $1, from 0 to $3.
# Leading zeros are dropped, so the shell never reads the number as octal.
whole_number_up_to() {
  case $2 in
    '' | *[!0-9]*) fail "Invalid value \"$2\" after the option $1" ;;
  esac
  number=${2#"${2%%[!0]*}"}
  number=${number:-0}
  if [ "${#number}" -gt "${#3}" ] || [ "$number" -gt "$3" ]; then
    fail "Invalid value \"$2\" after the option $1"
  fi
}

# Records the value $2 of the option $1.
record_option() {
  case $1 in
    --network) use_scenario "$2" ;;
    --lag)
      whole_number_up_to "$1" "$2" "$MAXIMUM_DELAY_MS"
      lag=$number
      ;;
    --jitter)
      whole_number_up_to "$1" "$2" "$MAXIMUM_DELAY_MS"
      jitter=$number
      ;;
    --loss)
      whole_number_up_to "$1" "$2" "$MAXIMUM_PERCENT"
      loss=$number
      ;;
    --duplicates)
      whole_number_up_to "$1" "$2" "$MAXIMUM_PERCENT"
      duplicates=$number
      ;;
    --reorder)
      whole_number_up_to "$1" "$2" "$MAXIMUM_PERCENT"
      reorder=$number
      ;;
  esac
}

# Reads the options of the network simulator from the arguments, in order, as
# the programs do: `--network` sets the five values of its scenario, every
# other option its own, so the last one to set a value wins. Sets lag, jitter,
# loss, duplicates and reorder; they are 0 when no option sets them.
read_conditions() {
  lag=0
  jitter=0
  loss=0
  duplicates=0
  reorder=0
  while [ "$#" -gt 0 ]; do
    case $1 in
      --network | --lag | --jitter | --loss | --duplicates | --reorder)
        [ "$#" -ge 2 ] || fail "Missing value after the option $1"
        record_option "$1" "$2"
        shift 2
        ;;
      *) fail "Unknown option $1" ;;
    esac
  done
}

# Prints the five values in the words of the server, such as "ping +60 ms, up
# to 20 ms more, 1 % lost, 0 % duplicated, 0 % reordered".
conditions_text() {
  printf 'ping +%s ms, up to %s ms more, %s %% lost, %s %% duplicated, %s %% reordered\n' \
    "$lag" "$jitter" "$loss" "$duplicates" "$reorder"
}

# Succeeds when every value is 0: there is nothing to simulate.
is_perfect_network() {
  [ "$lag" -eq 0 ] && [ "$jitter" -eq 0 ] && [ "$loss" -eq 0 ] &&
    [ "$duplicates" -eq 0 ] && [ "$reorder" -eq 0 ]
}

# Runs the command given, or prints it when the command of the script is
# `print`.
run() {
  if [ "$printing" = yes ]; then
    printf '%s\n' "$*"
  else
    "$@"
  fi
}

# Prints how to use the script that sources this file.
usage() {
  printf 'Usage: %s apply|print [options]\n' "${0##*/}"
  printf '       %s clear\n\n' "${0##*/}"
  printf 'apply  makes the loopback interface cross the network the options describe\n'
  printf 'print  prints the commands apply would run, and runs none\n'
  printf 'clear  removes what apply did\n\n'
  printf 'Options, read in order as the programs read them:\n'
  printf '  --network <name>        %s\n' "$(scenario_names | paste -s -d ' ' -)"
  printf '  --lag <ms>              what the ping gains, from 0 to %s\n' \
    "$MAXIMUM_DELAY_MS"
  printf '  --jitter <ms>           how much the ping may vary, from 0 to %s\n' \
    "$MAXIMUM_DELAY_MS"
  printf '  --loss <percent>        share of the datagrams lost\n'
  printf '  --duplicates <percent>  share of the datagrams that arrive twice\n'
  printf '  --reorder <percent>     share of the datagrams out of order\n'
}

# Runs the command of the script, $1, with the options after it. The script
# sets TOOL and INTERFACE, and defines apply_conditions and remove_conditions;
# apply and clear need root.
run_command() {
  printing=no
  case ${1:-} in
    print) printing=yes ;;
    apply | clear) [ "$(id -u)" -eq 0 ] || fail "run it as root: sudo $0 $*" ;;
    *)
      usage >&2
      exit 1
      ;;
  esac
  if [ "$1" = clear ]; then
    [ "$#" -eq 1 ] || fail 'clear takes no option'
    remove_conditions
    printf '%s removed from %s\n' "$TOOL" "$INTERFACE"
    return 0
  fi
  shift
  read_conditions "$@"
  if is_perfect_network; then
    fail 'nothing to simulate: give --network or a value, or use clear'
  fi
  apply_conditions
  [ "$printing" = yes ] || printf '%s on %s: %s\n' "$TOOL" "$INTERFACE" "$(conditions_text)"
}
