---
description: A screen that shows a later state never makes it wait on an earlier message that may be lost
trigger: always_on
---

# RULE: What a screen shows of a state never waits on an earlier message it can do without

## Rule

1. **Before gating a look on a message** (`if (!arrivedAt) return {};`), list each part of the look and the message it really needs. A part that the later message alone describes (the bar from the fight status) is computed from that message, never behind the earlier one (the Arriving status).
2. **Walk the case where the earlier message never arrives**: a guaranteed message is still lost past its resend window, and a client can join after it was sent. Write what the screen shows then; a part that disappears for good is the defect.
3. **A test feeds the later message alone** and checks that its part shows.

## Example

- ❌ **Before (wrong)**: `BossPresentation::look()` returned an empty look when no Arriving status had come, so a lost Arriving left the boss bar hidden for the whole fight.
- ✅ **After (right)**: the bar comes from the fight; the WARNING screen and the edges, which only the Arriving status times, show once it came. `GameScreenFixture.PaintsTheBossBarOnceTheBossFights` queues a fight status alone.
