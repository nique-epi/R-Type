---
description: Before a local of a Luau script is renamed or removed, every use of it is searched; a name no local declares reads nil and no build sees it
trigger: always_on
---

# RULE: A local removed from a Luau script is searched for every use — a missing name reads `nil`, silently

Why: `local LEFT` of `necromancer.luau` was replaced by `local UP` when the boss started to rise; the rings still waited with `direction = LEFT`, a global that reads `nil`, so they stood still, then flew back to the right. Nothing failed: no analyzer runs on the scripts.

## Rule

1. **Before renaming or removing a local of a script, search its uses**: `git grep -n -w '<NAME>' -- resources`.
2. **After editing a script, list the constants it uses that no local declares**, with this scan, and read every name it prints:
   ```bash
   python3 -I - resources/*/server/*.luau <<'PY'
   import re, sys
   for f in sys.argv[1:]:
       code = re.sub(r'"[^"\n]*"', '""', re.sub(r'--[^\n]*', '', open(f, encoding="utf-8").read()))
       declared = set(re.findall(r'local\s+([A-Z][A-Z0-9_]*)', code))
       for m in re.finditer(r'local\s+((?:[A-Za-z_]\w*\s*,\s*)+[A-Za-z_]\w*)\s*=', code):
           declared.update(n.strip() for n in m.group(1).split(','))
       print(f, sorted(set(re.findall(r'\b([A-Z][A-Z0-9_]{2,})\b', code)) - declared))
   PY
   ```
   Calibration: on the version of `necromancer.luau` without `local LEFT`, it prints `['LEFT']`.
3. **A behaviour whose body does not move is probed before it is reasoned about**: an `error(string.format(…))` in a copy of the script writes the values it sees in the journal of the game, once.

## Example

- ❌ **Before (wrong)**: `local LEFT = { x = -1, y = 0 }` replaced by `local UP = { x = 0, y = -1 }`; the ring's `behaviours.wait(…, { direction = LEFT })` leaves it at x 1353 for 1.17 s.
- ✅ **After (right)**: the scan prints `['LEFT']`, the local comes back, and a probe reads the ring at x 88 after the same wait.
