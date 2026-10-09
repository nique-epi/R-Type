# Assets

Every file the client reads at run time.

| Folder | Holds |
|---|---|
| `sprites/` | textures and sprite sheets |
| `sounds/` | short sound effects, loaded in memory |
| `music/` | music, read from disk while it plays |
| `fonts/` | fonts |

## Asset ids

Code names a file by its asset id: its path in this folder, with `/` between folders, for example `sprites/player_ship.png`. Ids are written once, as named constants.

Every folder and file name of an id starts with a lowercase letter `a-z` or a digit, then holds only `a-z`, digits, `_`, `-` and `.`. The same id then names the same file on Linux, macOS and Windows, and never names a file outside this folder.

## When files are loaded

Assets are loaded by a loading step that receives the ids the next screen needs. It first releases the loaded assets that screen does not list, then loads the missing ones one file at a time, so that a loading screen can be drawn between two files. An asset already loaded is never read again.

When an id breaks the rule, or its file is missing or cannot be read, the client stops after the step and names every one of them. Music is only located: its file is read while it plays.

## How the client finds this folder

The client looks for `assets/` next to its executable, then in the folder above it, where the Visual Studio generator writes `Release/` and `Debug/`. The folder it was launched from does not matter.
