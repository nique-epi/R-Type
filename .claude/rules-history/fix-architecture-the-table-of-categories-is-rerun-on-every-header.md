# RULE: The table of categories counts every header, and is written again when the folder or the rule changes

## Context

Pull request #199, the load-test bots (RTY-98). The plan moved the malformed-datagram tool from the root of `tests/tools/` into `tests/tools/MalformedDatagrams/` and wrote the bots in `tests/tools/LoadTestBots/`, both flat. On 4 November 2026 a teammate requested changes: "Dans le dossier tools/LoadTestBots c'est le bordel y'a trop de fichier au meme endroits", and the same for `MalformedDatagrams`, asking for sub-folders.

## Mistake

- **The bots.** The plan named the launch (`BotLaunch`, `BotLaunchOptions`), found it too small for a category, and said everything else served the play of the bots: one category, so the folder stayed flat under point 8 of the module rule. Later in the same pull request, at the user's request, the counting and the sending moved out of `Bot` into `BotTally` and `BotInputSender`, with their tests and fixtures. The folder then held two roles of four classes each: what a bot does in a game (`BotPilot`, `BotInputSender`, `BotTally`, `BotReport`) and the squad (`Bot`, `BotSquad`, `ReadyRule`, `BotStanding`). No table was written again, and the folder reached 36 files.
- **The malformed datagrams.** The plan counted "5 classes", the five `.cpp` files of the tool. Its headers declared 14 types in three roles: the datagrams written (`MalformedDatagramBuilder`, `MalformedDatagramKind`, `SampleMessage`, `BoundedByte`, `MalformedDatagramConstants`), the flood sent to a server (`DatagramFlood`, `FloodRecipient`, `FloodReport`, `LoopbackPeer`, `SessionOpening`) and the launch (`FloodLaunch`, `FloodLaunchOptions`, `FloodMode`).
- **The rule changed under the branch.** Points 15 to 18 of the module rule (#194: how fine a category is, the helper types that count, the table to write) reached the branch through a merge of `main`; the layout was not checked again against them.

## Root cause

The table was written once, in the plan, from the `.cpp` files, and never again: neither when an extraction added classes to the folder, nor when a merge brought a newer version of the rule that decides the layout.

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
