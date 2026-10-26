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

### An absence is proven by grep's own exit code

1. **To prove a pattern is absent, run `git grep -q '<pattern>' -- <paths>` alone and read `$?`**: 1 means no match, 0 a match, anything else an error. After `| head` or `| wc -l`, `$?` is the code of `head` or `wc`, always 0.
2. **Never print `exit $?` after a pipe in a check meant to be read later**: the printed line looks like evidence and says nothing about the command before the pipe.

## Example

- ❌ **Before (wrong)**: `cmake --build build 2>&1 | tail -15; echo $?` → prints 0 even when compilation fails.
- ✅ **After (right)**: `cmake --build build > /tmp/build.log 2>&1; echo "exit: $?"` → real code (non-zero on failure), then read the log.
- ❌ **Before (wrong)**: `git grep -n 'GameEvent' -- src/client | head -3; echo "exit $?"` → empty output and `exit 0`, which reads as "found".
- ✅ **After (right)**: `git grep -q 'GameEvent' -- src/client; echo "grep exit: $?"` → `1`, no match.
