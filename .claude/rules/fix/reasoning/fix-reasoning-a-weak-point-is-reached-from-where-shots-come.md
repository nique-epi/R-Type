---
description: A weak point judged in pre_hit is reachable along the path of a missile, which touches the first box on its way, not the point aimed at
trigger: always_on
---

# RULE: A weak point is judged where a missile first touches the body, not where the weak point is drawn

Why: the Necromancer let a hit through only within 60 units of the column of its mask; a missile comes from the left and touches the front of the hood box first, about 100 units left of the mask, so it was always stopped and the boss never lost a point.

## Rule

1. **Before writing a `pre_hit` that judges `hit.offset`, walk a missile along its path**: it flies from the left (the right for a missile of a Bydo) and the hit is judged on the first tick its box overlaps a box of the body. Compute that offset for each row the weak point covers.
2. **A weak point behind the front edge of a box is judged by its rows only**, or the box is cut so the weak point is its front.
3. **Measure it before saying the body can be hurt**: bots fight it and the tally of the fight (`boss at N % health`) or a probe of `e.health` after some attacks shows it lost points.

### A body is hurt wherever it is drawn, unless the design names its armour

1. **A boss takes a hit on every part the player reads as its body** (head, thorax, abdomen, wings, spikes); only a part the design names as armour or as a limb (legs, scythes, a shield) stops a missile. A weak point narrower than the body is a design choice for the user, never a default.
2. **Draw the boxes over a frame of the sheet before the fight is played**: every opaque part that is body lies in a box, and a limb lies in none, so a missile flies through it.
3. **A `pre_hit` then judges time** (the arrival, a strike), not rows, when the whole body counts.

## Example

- ❌ **Before (wrong)**: `math.abs(hit.offset.x - MASK.x) <= MASK.half_width` with the hood box 105 units wide on each side of the mask → every hit at x about -100 is stopped; bots end both fights with the boss at 100 %.
- ✅ **After (right)**: `math.abs(hit.offset.y - MASK.y) <= MASK.half_height` → the boss has 430 of its 450 points after six attacks against bots.
- ❌ **Before (wrong)**: the Ash Mother let a hit through only on the 144 rows of her maw, and her spikes and wings had no box; the user found the hitbox too small.
- ✅ **After (right)**: boxes over her head, body, spikes and wings, none on her legs, and a `pre_hit` that only refuses during her arrival and her strikes; bots defeat her in 29 s.
