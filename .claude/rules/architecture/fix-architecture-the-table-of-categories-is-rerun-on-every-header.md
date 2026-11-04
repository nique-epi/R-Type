---
description: "The table of categories of a folder counts every header of a type, and is written again whenever a commit adds a class to the folder or a merge changes the module rule"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: The table of categories counts every header, and is written again when the folder or the rule changes

Why: `tests/tools/LoadTestBots/` stayed flat after two classes were extracted into it, and `tests/tools/MalformedDatagrams/` was counted as five classes from its `.cpp` files; a review found 36 and 25 files side by side.

## Rule

1. **Count the types of a folder from its headers, never from its `.cpp` files**: a plain struct, an enum and a constants header are classes of point 8 of `architecture-a-module-is-one-folder-and-one-target.md`, and point 16 counts them with the class they serve. List them with
   ```bash
   git ls-files -- '<folder>/*.hpp' | grep -v -E '(Test|Fixtures?|TestConstants)\.hpp$'
   ```
   then group the names into roles. Calibration: on `e6ba6f4d`, with `git ls-tree -r --name-only e6ba6f4d -- tests/tools/MalformedDatagrams` piped into the same `grep`, it prints 14 headers, where the folder holds 5 `.cpp` of classes.
2. **Write the table of point 18 again, before the commit, whenever a commit adds a class to a folder the change adds or flattens**: an extraction that moves code out of a class into new ones changes the count of the folder it lands in.
3. **Write it again after every merge that changes the module rule**, as `fix-process-reread-rules-after-switching-branch.md` reads the rules again: a layout chosen under the older version is checked under the newer one before the next push.
4. **A role of two classes is not a category**: its classes go into the category that uses them (point 17 of the module rule), or the role gains its constants header when it has one of its own (point 16). Never a sub-folder of two classes.

## Example

- ❌ **Before (wrong)**: the plan counts the bots as the launch (two files) and "everything else serves the play", keeps the folder flat, then extracts `BotTally` and `BotInputSender` into it without a new table; the malformed-datagram tool moves as "5 classes", its 14 headers never listed.
- ✅ **After (right)**: the headers are listed and grouped: `LoadTestBots/Launch/` (`BotLaunch`, `BotLaunchOptions`, `BotLaunchConstants`), `Play/` (`BotPilot`, `BotInputSender`, `BotTally`, `BotReport`, `BotPilotConstants`) and `Squad/` (`Bot`, `BotSquad`, `asksReady()`, `BotStanding`, `BotSquadConstants`); `MalformedDatagrams/Builder/`, `Flood/` and `Launch/`, with `FloodConstants.hpp`, which all three read, at the root.
