# RULE: A weak point is judged where a missile first touches the body, not where the weak point is drawn

## Context

The boss of the level fosse, the Necromancer, was shrunk and made to rise from below; the user reported that its attacks hurt nothing and that it took no damage.

## Mistake

the Necromancer let a hit through only within 60 units of the column of its mask; a missile comes from the left and touches the front of the hood box first, about 100 units left of the mask, so it was always stopped and the boss never lost a point.

## Root cause

The script was changed and reasoned about without being run against a measurement; Luau reports no missing name and no unreachable point.

## Rule

1. **Before writing a `pre_hit` that judges `hit.offset`, walk a missile along its path**: it flies from the left (the right for a missile of a Bydo) and the hit is judged on the first tick its box overlaps a box of the body. Compute that offset for each row the weak point covers.
2. **A weak point behind the front edge of a box is judged by its rows only**, or the box is cut so the weak point is its front.
3. **Measure it before saying the body can be hurt**: bots fight it and the tally of the fight (`boss at N % health`) or a probe of `e.health` after some attacks shows it lost points.

## Example

- ❌ **Before (wrong)**: `math.abs(hit.offset.x - MASK.x) <= MASK.half_width` with the hood box 105 units wide on each side of the mask → every hit at x about -100 is stopped; bots end both fights with the boss at 100 %.
- ✅ **After (right)**: `math.abs(hit.offset.y - MASK.y) <= MASK.half_height` → the boss has 430 of its 450 points after six attacks against bots.

## Update (2026-11-20) — A body is hurt wherever it is drawn

The Ash Mother let a hit through only on the rows of her maw (64 ± 72 units), and her spikes and wings had no box; the user said her hitbox was too small and asked for the whole boss to be hittable except her legs. The maw window had been chosen as the weak point without the user ever asking for one. Her boxes now cover her head, body, spikes and wings, none her legs, and her `pre_hit` refuses only during her arrival and her strikes.
