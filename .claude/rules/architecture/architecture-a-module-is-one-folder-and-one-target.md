---
description: "A module is a feature one can switch on, off or replace: one folder and one target around one IModule or IGameModule class, which owns all the folder holds; a folder without that class is a library"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A module is one folder and one target around one module class; a folder without that class is a library

Why: the subject (page 16) makes an engine modular through what a developer chooses to compile, link or load, so the word must name what can be left out; a word that names any folder with a target says nothing of that.

## Rule

### What a module is

1. **A module is a feature one can switch on, switch off or replace.** It is a folder with one class at its root implementing `IModule` (it lives as long as its program) or `IGameModule` (as long as one game). Everything the folder holds belongs to it, sub-folders included: the class builds those objects and hands them to the others with `provide()`.
2. **A folder without that class is a library: code with no life cycle** (`src/client/Widgets/`, `src/game/Waves/`). A system (`ISystem`) and a service are not modules: a list of systems runs the first, a module provides the second.

### Required or optional

3. **Every module is required or optional, and the list of modules in `docs/src/content/docs/project-layout.md` says which.** A required module may be included by the code that needs it. An optional module is reached only through its `Contract/` sub-folder, which stays compiled when the module is removed. In `src/`, the class of a module is included by the list that names it (`serverModules()`, `clientModules()`, `rtypeGameModules()`) and nothing else.
4. **The removal test, for an optional module**: remove its folder, except `Contract/`, and its line in the list; the program compiles, starts, and its binary is smaller. **No module passes it yet**; `project-layout.md` says what stops the sound and the metrics.

### The code of a module used elsewhere

5. **When another folder needs code of a module, the first row that fits decides**, and a link never points back at a folder that links its folder (`fix-architecture-a-link-never-points-back-at-a-folder-that-links-it.md`):

   | Order | The code… | Then |
   |---|---|---|
   | 1 | belongs to a required module | it stays there, and the code that needs it includes it: `Lobby/` and `Metrics/` include `Games.hpp`, `Widgets/` includes `PlayfieldMapping.hpp` |
   | 2 | belongs to an optional module | it goes in the `Contract/` of that module: `ISoundPlayer` |
   | 3 | has no owner and serves several folders | it goes in a library: `Widgets/`, `Rendering/` |

### Inside a folder

6. **A folder has one `CMakeLists.txt` declaring one target** named `rtype_<owner>_<folder in snake_case>`, ended by `_module` for a module (`rtype_server_metrics_module`). It lists the `.cpp` of every class of the folder and exposes the folder (`fix-architecture-one-cmakelists-per-folder.md`).
7. **Inside a folder, everything is a file of that folder or of the sub-folder of its category (point 8), never a folder of its own**: a concrete class, an abstract class, free functions, an exception, an interface, a plain struct, a constants header. One class per `.hpp`/`.cpp` pair, named after it (`code-style-file-named-after-its-class.md`); a helper class a class needs is a pair of the same folder, never a type nested in it (`fix-architecture-no-type-nested-in-a-class.md`).
8. **A folder whose classes fall into categories has a sub-folder for each.** A category is a group of at least three classes that change for the same reason and that a reader names in a few words: in `src/engine/Commands/`, the interpreter (the table of commands and the cutting of a line), the settings (the declared settings and `set`) and the configuration (reading `server.cfg` and `client.cfg`). The per-platform files of one class (`BorderlessScreenLinux.cpp`, `…MacOs.cpp`, `…Windows.cpp`) count as one class. What serves the whole folder stays at its root: the module class, the constants header and the interfaces several categories use. A folder with fewer than two categories stays flat, whatever it holds: there is no count of files to reach, the name of the category tells the reader where to look.
9. **A sub-folder exists only in three cases**, with a `CMakeLists.txt` and a target of its own:
   - it is a category (point 8);
   - a group of classes needs a dependency the rest must not link (a file parser, a platform API);
   - it is the `Contract/` of an optional module.

   Never one per class, and never deeper than three levels: a sub-folder has sub-folders of its own only when it meets point 8 itself, at least two categories of three classes (`Menus/Options/` splits into the screen with its dialogs and the tabs with their rows), and a folder of the third level has none. At every level, the parent links the sub-folder and never the other way round.
10. **Two classes of a folder never include each other**, directly or through a third, since the build no longer stops them: when both need the same thing, it moves to an interface or a plain struct they both include.
11. **Tests mirror the folder** (`tests/server/Spawning/`), one test file per class; a test double stays in `tests/doubles/`.

### A new module, and what stays tolerated

12. **A new module is written directly in its own folder, `src/<program>/<Name>/`**, class and code together. `Modules/` receives nothing but its line in the list, and `ModuleConstants.hpp` the constant of its name. Its pull request adds its row to the list of modules, required or optional.
13. **Only the class folders of `src/client/Widgets/`, `src/client/Rendering/` and of the libraries `src/engine/`, `src/game/`, `src/network/`, `src/logging/` and `src/concurrency/` stay tolerated**, until each is flattened in a change of its own; the client and the server are tidied. Such a folder holds either class folders or flat pairs, never both. A change that adds a class to it either gives it a folder like its siblings, or flattens the folder first in its own `refactor` commit (includes unchanged, targets merged, `CMakeLists.txt` and docs updated, `fix-process-grep-every-page-for-what-the-feature-replaced.md`); the pull request says which. A folder the change does not touch is reported, not reworked.
14. **The layout holds through this rule, the map and review.** No CI job checks it, and `rtype_forbid_links` gets no check for it. Before the push, name the categories of each folder the change adds or flattens, and check that every category of three classes or more has its sub-folder (point 8).

### How fine a category is

15. **A category is a role a reader names, never a single trigger of change.** Group the classes as the example of point 8 does: "the interpreter" holds the table of commands and the cutting of a line, two mechanisms with one role. In `src/server/Rules/`, the lives, the score and the steering of a ship are one category, what happens to each player in a match, not three groups of one or two classes.
16. **The helper types of a class count with it** toward the three classes of a category: its `…Parts` struct, its constants header, a plain struct only it builds or reads. A class is never "alone" while it shares a role with its neighbours.
17. **A class two categories share never keeps the folder flat.** It goes into the sub-folder of the category that needs it first, and the other sub-folder includes it there, the two including each other in one direction only; or it goes into `Contract/` when other folders read it too. Only the interfaces and the constants header of point 8 stay at the root.
18. **Before moving the first file, write the table of the categories of each folder the change adds or flattens**: the folder, each category with its classes, the result. It goes in the plan or the pull request. A table where most folders keep one category and stay flat is the symptom of a reading finer than point 8: reread it at the grain of `Commands/` before moving anything.

### A feature of the master is laid out like a web backend

19. **A folder of a feature of the master** (`src/master/<Feature>/`) holds its module class at its root, and one sub-folder per role, as a module of Yuno backend does, whatever the number of classes:

    | Sub-folder | Holds | Links |
    |---|---|---|
    | `Controllers/` | one class that reads the requests of the routes and writes their answers | `Services/` |
    | `Services/` | the rules; `Services/Helpers/` for pure functions with no I/O | `Repositories/` |
    | `Repositories/` | the only code with the SQL of the tables of the feature, one class per table | `Models/` |
    | `Models/` | the plain structs the layers hand each other | nothing of the feature |

    A sub-folder exists only when the feature has a class of that role. Points 7 to 9 do not apply to these folders: the role, not the count, makes the sub-folder.
20. **What the master and the game server both read** (`src/network/Master/`) is laid out like the `common/` of a cRPC: `Routes/` (the method and path of each route, declared once), `Messages/` (one plain struct per message), `Validators/` (reading, checking and writing each message), `Client/` (the class the game server calls). The master and the client use the same declarations, so they cannot disagree on a route or a member.

## Example

- ❌ **Before (wrong)**: `src/engine/Commands/` with its 26 files side by side: the command table, the cutting of a line, the settings, the reader of the file and its report, so a reader opens a list of names to find where `set` is.
- ✅ **After (right)**: `Commands/Interpreter/`, `Commands/Settings/` and `Commands/Configuration/`, three folders of three to four classes each, each with its target; the `Commands/` target only links the three, so no caller changes.
- ❌ **Before (wrong)**: the tidying of the server counts the lives (`LifeKeeper`, `PlayerLives`), the score (`ScoreKeeper`) and the steering of a ship (`PlayerController`, `ShipInput`) of `src/server/Rules/` as three reasons to change, finds a single category of four classes, and keeps the ten classes flat.
- ✅ **After (right)**: `Rules/Match/` (`GameFlow`, `StartBarrier`, `WorldSlot`, `RtypeSystems`) and `Rules/Players/` (the lives, the score and the ship of each player), with `RtypeRulesModule` at the root; `GameFlow` includes `LifeKeeper` and `PlayerController`, so `Match/` includes `Players/`, and `Players/` includes nothing of `Match/`.
- ❌ **Before (wrong)**: `src/game/Waves/` called a module because it has a target, and a new tick report written in `src/server/Modules/TickReport/`.
- ✅ **After (right)**: `src/game/Waves/` is a library, since no class of it implements `IModule`; the tick report is written in `src/server/TickReport/`, with a line in `serverModules()` and a row in the list of modules.
- ❌ **Before (wrong)**: the class `ClientAudioModule` in `src/client/Modules/Audio/`, its code in `src/client/Audio/`, and `ISoundPlayer.hpp` next to the classes only the module needs, so nothing tells what stays when the sound is removed.
- ✅ **After (right)**: the shape the sound has since the client is tidied:
  ```
  src/client/Audio/            the module audio, optional
  ├── CMakeLists.txt           add_library(rtype_client_audio_module STATIC AudioService.cpp ClientAudioModule.cpp)
  ├── ClientAudioModule.hpp
  ├── ClientAudioModule.cpp
  ├── AudioService.hpp
  ├── AudioService.cpp
  ├── Sounds/                  the players of the sounds
  ├── Music/                   the player of the music
  └── Contract/                what other folders may include, compiled without the module
      ├── CMakeLists.txt       add_library(rtype_client_audio_contract INTERFACE)
      ├── Service/             IAudioService.hpp, …
      ├── Sounds/              ISoundPlayer.hpp, …
      └── Music/               IMusicPlayer.hpp, …
  ```
  `rtype_client_audio_module` links `Contract/`; `Contract/` links nothing of `Audio/` outside itself.
