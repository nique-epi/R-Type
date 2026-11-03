# History: a module is one folder and one target

## Context

The rule arrived on 28 October 2026 with pull request #139 (commit `8c0b8fa6`), which assembled the server from modules. It replaced `fix-architecture-a-cpp-hpp-pair-has-its-own-folder.md`: a folder and a `CMakeLists.txt` per class had made 14 of the 70 files of the waves pull request pure wiring, so the user chose a folder with flat pairs and one target as the unit. The older rule's history records that change under its own `Update (2026-10-28)`. This rule had no history file of its own until now.

## Mistake

That version gave the word "module" to the unit of layout: "The word names a folder and a target: it is not `IModule`, and a module folder may hold a class implementing `IModule` or none." `modules.md` then had to say that the word has two meanings in the repository. `logging.md` used a third one, the name a `Logger` is given.

The decision page of 26 October 2026 had another definition: a module is what one wants to switch on, switch off or replace. On 3 November the user asked to read the subject before defining a word the project is graded on. Page 16 lists three ways to build a modular engine (compile time, link time, a run-time plugin API), so that it takes no more disk or memory than needed.

## Root cause

The word was defined from the code as it stood and from what a folder looks like, not from the subject nor the decision page. Measured on `a5adf3b9`, reading the code: one CMake option (`BUILD_TESTS`), no `SHARED` or `MODULE` target, no `dlopen` or `LoadLibrary`, no call to `find<>()` outside `InstallContext` itself. Nothing could be left out of a program, and no rule pointed at that.

## Rule

See the rule file of the same name in `.claude/rules/architecture/`: a module is one folder with one `IModule` or `IGameModule` class at its root; a folder without that class is a library; a required module may be included, an optional one is reached through its `Contract/`; the removal test; where the code of a module used elsewhere goes; when a sub-folder exists.

Decisions of the user on 3 November 2026, kept with the rule:

- no CI job for the layout: it holds through the rule, the map and review, and `rtype_forbid_links` gets no new check;
- `Modules/` keeps only the list of the program;
- the sound, the window and the rendering stay in `src/client` while track 1 is undecided, although the modules of the subject are those of the engine;
- moving the code (RTY-219, RTY-220) is separate from making a module removable (RTY-222, RTY-204) and from removing `screen-registry` (RTY-221).

Engine², which the kick-off cites, has the same shape: a core, and a folder per feature with its class next to its code (<https://github.com/EngineSquared/EngineSquared>). Bevy does the same with its plugins.

## Example

- ❌ **Before (wrong)**: `src/server/Spawning/` called a module because it has a target, while its code has no life cycle and nothing can switch it off.
- ✅ **After (right)**: `src/server/Spawning/` is a library; `src/server/Modules/Metrics/`, with `ServerMetricsModule`, is a module, listed as optional in `project-layout.md`.

## What the rewrite of 3 November dropped

- Point 3 of the old version measured the number of `.cpp` per folder on `origin/feat/server-modules` at `3ad9929`, not on `main`. The figures say nothing of `main` today, so they are gone.
- The command of the old point 8 is gone. It counts only the `.cpp` directly in a folder: on `a5adf3b9` it prints nothing, and `src/client/Widgets`, with 39 `.cpp` in class folders, never shows. Counting the folders a change adds or flattens does the same job.
- "Another module reuses the group" no longer justifies a sub-folder: reused code stays in a required module, goes in a `Contract/` or goes in a library.
- `project-layout.md` said the classes only one module uses sit in the folder of that module, with `FrameLoop` and `Games` as examples. At `da8b3256`, `FrameLoop` is included by `Modules/Assembly`, `Modules/Screens` and `main.cpp`, `Games` by `Modules/Lobby` and `Modules/Metrics`. The sentence is gone; the classes move when the client and the server are tidied.

## Update (3 November 2026) — folders by category instead of a count of ten

### Context

`src/engine/Commands/` holds eight `.cpp` and twenty-six files with the headers. The count of ten `.cpp` kept it flat, although its classes fall into three groups a reader names at once: the interpreter, the settings and the reading of the configuration file. A reviewer asked for sub-folders, and the author of the folder, who had split it into three and then flattened it to follow point 9, said a category is always more visible and cleaner than a count.

### What was measured

On `origin/main` at `68e69e74`, counting the `.cpp` and `.hpp` files of each folder of `src/`: 355 folders, 4 with more than 20 files, none with more than 30, and one with more than 8 `.cpp` (`src/client/Platform`, 9). A rule by category therefore touches few folders. Only `Commands/` was read to check that its categories are real: whether the other large folders (`Widgets/`, `Components/`, `Screens/`, `Rendering/`) split into categories of three classes or more was not measured.

### Decision

- The count of ten `.cpp` is gone from points 8 and 9. A folder whose classes fall into categories has a sub-folder for each; a category is a group of at least three classes that change for the same reason.
- The number three is a choice of the author, not a measurement; it keeps a sub-folder from holding one or two classes, the case the first version of the rule feared with a folder per class.
- The check of point 14 names the categories instead of counting files, so it is a review, not a command. No command can decide that three classes change for the same reason.
- Sub-folders stay two levels at most, and the three cases of point 9 stay the only ones.

## Update (3 November 2026) — how fine a category is

### Context

The tidying of the server (pull request #192) and of the client (#193) applied points 8 and 14 right after the count of ten `.cpp` gave way to categories. The server kept every folder flat but `Sessions/`. The client split `Connection/`, `Menus/` and `Replication/`, and kept `Audio/`, `Window/`, `Screens/`, `Input/`, `Settings/` and `Assets/` flat. The user found almost no sub-folder in either.

### Mistake

Both tables of categories read "change for the same reason" at the finest grain:

- In `src/server/Rules/`, the lives (two classes), the score (one) and the steering of a ship (two) were three reasons, so the folder had a single category, the course of a match, and stayed flat.
- In `src/client/Audio/`, the sounds and the music were three types each only when a helper struct counted, so they were not counted; both also read `AudioVolume`, which a sub-folder cannot include from its parent, so the folder stayed flat.
- In `src/client/Window/`, `EventListeners` and its two helper types were one class, and `FrameLoop` was alone.

### Root cause

Point 8 gives one example of the grain, `Commands/`, and says nothing of the helper types of a class nor of a class two categories share. The briefs of both changes relayed the rule without that grain, and the first table of categories was not read before the files moved.

### Rule

Points 15 to 18 of the rule: a category is a role a reader names; the helper types of a class count with it; a class two categories share goes down into one of them or into `Contract/`, and never keeps the folder flat; the table of categories is written before the first file moves, and a table of flat folders is read again at the grain of `Commands/`.

## Update (3 November 2026) — a third level of folders

### Context

Reading the tidied client, the user asked why `Connection/Session/` does not group what belongs to the session apart from the handshake, and said the same holds for the options and everywhere. `Connection/Session/` could be split at the level of `Connection/` (`Session/`, `Handshake/`, `WaitingRoom/`) within two levels. `Menus/Options/` could not: its two categories, the screen with its dialogs (`OptionsScreen`, `ResetDialog`, `SavedStamp`) and the tabs with their rows (`OptionsTabs`, `OptionsRows`, `OptionsRowHandles`, `OptionsDescriptions`), sit inside a sub-folder, and point 9 forbade a sub-folder of a sub-folder.

### Decision

The user allowed a third level on 3 November 2026. A sub-folder splits again only when it meets point 8 itself, at least two categories of three classes, and the parent links its sub-folders at every level. A folder of the third level never splits.
