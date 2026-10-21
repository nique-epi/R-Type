---
description: A unit an indicator shows as lost at zero is shown as lost at every count, never just removed
trigger: always_on
---

# RULE: A unit shown as lost when none is left is shown as lost at every count

## Rule

1. **Before drawing an indicator made of units** (lives, ammunition, shields, slots), write how it looks at each count, from the full count down to zero, side by side.
2. **If the empty state shows the units as lost** (greyed, ghosted, crossed out), **every intermediate count shows the units already lost the same way**: one lost unit for each one gone, in the same place. A unit that vanishes at 2 but comes back as a ghost at 0 is an inconsistency.
3. **Pick one model and say it in the design**: a fixed number of slots, filled then lost; or a plain count that shrinks to nothing. A design never mixes the two.
4. **A state that hides the count** (unknown before the first message, a count too large for its slots) is drawn apart and named in the design, so it is not read as "all lost".

## Example

- ❌ **Before (wrong)**: the status band drew one ship per life left and nothing for the lives lost, but three ghost ships for a pilot out of lives: a pilot on its last life showed one ship, then three ghosts once dead.
- ✅ **After (right)**: three slots of lives, a ship of the player color for each life left and a ghost ship for each life lost, at every count; unknown lives draw nothing, more than three draw one ship and their count.
