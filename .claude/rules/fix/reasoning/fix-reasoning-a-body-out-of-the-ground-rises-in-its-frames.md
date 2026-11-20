---
description: A boss that comes out of the ground jumps to its rest and rises in frames cut at the ground, never by moving up from below the window
trigger: always_on
---

# RULE: A body that comes out of the ground rises in its frames, cut at the ground — it never moves up from below the window

Why: the Sandmaw and the Ash Mother climbed from below the window to their rest; the client draws a body over the decor, so they slid up across the floor of the map instead of out of it.

## Rule

1. **A body whose arrival comes out of the floor of the level is not moved there.** It waits below the window through its warning, then its script asks `jumps_to` its rest in the same intent that `starts_action` its arrival, as `necromancer_rite` does.
2. **The rise is in its sheet**: an arrival animation of the size of its idle, `playback = "hold"`, `afterwards = "stay"`, whose frames show the ground of the frame first, then the body coming up out of it, every row under the top of the ground cut. Its frames times `frame_ms` equal the `rise_ms` the script waits.
3. **Look at the arrival in the game**, the body against the floor of the map: a body drawn sliding over the floor is the defect, whatever the logs say.

## Example

- ❌ **Before (wrong)**: `behaviours.wait(rise_ms, { direction = UP, top_speed = climb, starts_action = arrival })` from y 1160 → the Sandmaw slides up across the bottom of the window and over the floor.
- ✅ **After (right)**: `behaviours.wait(rise_ms, { direction = STILL, jumps_to = { x = e.position.x, y = settings.rest_y }, starts_action = arrival })`, with `[appearance.attack_2.signal]`, 14 frames of 100 ms coming up out of its dune.
