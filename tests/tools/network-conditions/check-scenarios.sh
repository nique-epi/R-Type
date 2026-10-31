#!/bin/sh
# Checks that the scenarios and the bounds of conditions.sh are those of the
# server, value for value, so the scripts never drift from the network
# simulator of the game: each scenario as its help lists it, each bound as its
# options take it. Registered in CTest as network_condition_scenarios.
#
#   tests/tools/network-conditions/check-scenarios.sh ./r-type_server
set -eu

# shellcheck source-path=SCRIPTDIR source=conditions.sh
. "$(dirname "$0")/conditions.sh"

[ "$#" -eq 1 ] || fail 'usage: check-scenarios.sh <r-type_server>'
server=$1
help=$("$server" --help)
status=0

# Writes that the check $1 passed, or failed when $2 is not 0.
report() {
  if [ "$2" -eq 0 ]; then
    printf 'ok: %s\n' "$1"
  else
    printf 'FAILED: %s\n' "$1"
    status=1
  fi
}

listed=0
while read -r name; do
  use_scenario "$name"
  scenario="$name ($(conditions_text))"
  printf '%s\n' "$help" | grep -q -F -e "$scenario" && found=0 || found=1
  report "the help lists $scenario" "$found"
  listed=$((listed + 1))
done <<EOF
$(scenario_names)
EOF

in_help=$(printf '%s\n' "$help" | grep -o -F -e '(ping +' | grep -c . || true)
[ "$in_help" -eq "$listed" ] && same=0 || same=1
report "the help lists $in_help scenarios, the scripts $listed" "$same"

# Each option takes its bound and refuses the value after it.
for bound in "--lag $MAXIMUM_DELAY_MS" "--jitter $MAXIMUM_DELAY_MS" \
  "--loss $MAXIMUM_PERCENT" "--duplicates $MAXIMUM_PERCENT" \
  "--reorder $MAXIMUM_PERCENT"; do
  option=${bound% *}
  largest=${bound#* }
  if "$server" "$option" "$largest" --help > /dev/null 2>&1 &&
    ! "$server" "$option" "$((largest + 1))" --help > /dev/null 2>&1; then
    taken=0
  else
    taken=1
  fi
  report "the server takes $option up to $largest" "$taken"
done
exit "$status"
