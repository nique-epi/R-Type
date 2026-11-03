---
description: A thread that drains two queues takes first the queue pushed to last, or a later push is served before the earlier one it depends on
trigger: always_on
---

# RULE: Two queues are drained against the order of their pushes

Why: a producer pushes to A then to B, and the consumer drains A then B: both pushes can fall between the two drains, so the consumer serves B without having seen A.

## Rule

1. **When a consumer must see A before it serves B, and a producer pushes to A then to B, it drains B first, then A, then works through A, then through B.** Whatever it took from B was pushed after what it then finds in A.
2. **Walk the interleaving by hand before writing it**: place the two pushes of the producer between each pair of steps of the consumer, and write what it serves.
3. **Say the order and its reason in the doc comment of the consumer**: nothing in the code shows it, and a test can only catch it by chance.

## Example

- ❌ **Before (wrong)**: `ResultsArchive::workThrough` drains the results, saves them, then drains the requests: a result and the request after it, both pushed between the two drains, answer without that result.
- ✅ **After (right)**: it drains the requests, then the results, saves the results, then answers the requests.
