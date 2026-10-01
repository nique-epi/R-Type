---
description: Never read a command's exit code through a pipe — redirect to a file and read $? immediately
trigger: always_on
---

# RULE: Getting the exit code of a piped command in zsh (not `PIPESTATUS`)

## Rule

1. **Never judge a command's success through a pipe.** `$?` after a pipe is the one of the **last** command (`tail`, `grep`), not the one being tested.
2. To get the real code: run the command **without a pipe**, redirect the output to a file, read `$?` immediately, then filter the file:
   ```bash
   cmake --workflow --preset test > /tmp/test.log 2>&1; echo "exit: $?"
   tail -30 /tmp/test.log
   ```
3. If reading a pipe status is unavoidable in zsh: `$pipestatus[1]` (lowercase, 1-indexed), never `${PIPESTATUS[0]}`.
4. A gate is only announced green with **the output and the code** read correctly.

## Example

- ❌ **Before (wrong)**: `cmake --build build 2>&1 | tail -15; echo $?` → prints 0 even when compilation fails.
- ✅ **After (right)**: `cmake --build build > /tmp/build.log 2>&1; echo "exit: $?"` → real code (non-zero on failure), then read the log.
