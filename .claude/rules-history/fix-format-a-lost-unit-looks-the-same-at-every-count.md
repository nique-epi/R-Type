# RULE: A unit shown as lost when none is left is shown as lost at every count

## Context

The status band of the game (RTY-151) shows the lives of each pilot as small ships. The design system proposed it, the user validated it as a picture, and it was coded and opened as a draft pull request.

## Mistake

The band drew one ship of the player color per life left and nothing for a life lost, but three ghost ships, the silhouette of a life in the shade of the far stars, for a pilot out of lives. A pilot going from 3 lives to 1 saw two ships vanish, then three ghosts appear when it died. The user pointed out the inconsistency on the picture posted on the pull request.

## Root cause

Each state was designed on its own: "lives left" as a shrinking count, "out of lives" as an empty row of slots. Nobody laid the counts from 3 down to 0 side by side, where the two models visibly clash.

## Rule

Before drawing an indicator made of units, write how it looks at each count from full to zero; if the empty state shows lost units, every count shows its lost units the same way. One model per indicator, and the states that hide the count (unknown, over the slots) are drawn apart.

## Example

- ❌ **Before (wrong)**: one ship per life left, nothing for a lost life, three ghosts at zero.
- ✅ **After (right)**: three slots, a ship per life left and a ghost per life lost, at every count; unknown lives draw nothing, more than three draw one ship and the count.
