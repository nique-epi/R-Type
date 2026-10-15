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

The assets of the interface, which any screen may use at any time, in a game or outside one, are loaded once at launch and never released. The assets of the title screen, its logo and the entity sheet of its squadron, are loaded at launch too, so the first frame shows the title, but they are not kept: entering a game releases the logo. The assets of a game are loaded by the loading screen when the player enters it: it first releases the loaded assets the game does not list, except the interface ones, then loads the missing ones one file per frame, drawing its progress between two files. An asset already loaded is never read again.

When an id breaks the rule, or its file is missing or cannot be read, the client stops once every file of the list was tried, and names every one of them. Music is only located: its file is read while it plays.

## How the client finds this folder

The client looks for `assets/` next to its executable, then in the folder above it, where the Visual Studio generator writes `Release/` and `Debug/`. The folder it was launched from does not matter.

## Where the files come from

| File | What it holds | Source |
|---|---|---|
| `fonts/press_start_2p/press_start_2p.ttf` | Press Start 2P, the pixel font of the menus: bars, headings, lines of text | SIL Open Font License 1.1, copyright 2012 The Press Start 2P Project Authors, license in `fonts/press_start_2p/OFL.txt`; from `google/fonts` (`ofl/pressstart2p`) |
| `fonts/silkscreen/silkscreen.ttf` | Silkscreen, the small pixel font of the menus: version, copyright | SIL Open Font License 1.1, copyright 2001 The Silkscreen Project Authors, license in `fonts/silkscreen/OFL.txt`; from `google/fonts` (`ofl/silkscreen`) |
| `sprites/logo.png` | The logo of the title screen, 210 × 42 pixels | Drawn for this project (main menu mockup 1c v4) |
| `sprites/entities.png` | The sprite sheet of a match: four player ships, the Bydo, the bonus Bydo, the player missile and the enemy missile | Drawn for this project (proposal 3a); not taken from the sprite set shipped with the subject |

## The entity sheet

`sprites/entities.png` is 66 × 140 pixels and holds every sprite at its native size: one pixel of the sheet is one pixel of the background grid, which covers 4 × 4 logical units of the playfield. Each entity holds one row with its two animation frames, 2 pixels apart; rows are 2 pixels apart too, and everything outside the frames is transparent.

| Entity | Native size | Frame time |
|---|---|---|
| Player ship, 4 colors, one row each | 32 × 16 | 80 ms |
| Bydo | 24 × 24 | 250 ms |
| Bonus Bydo | 24 × 24 | 200 ms |
| Player missile | 16 × 6 | 60 ms |
| Enemy missile | 8 × 8 | 120 ms |

Only the squadron of the title screen animates the frames; the game shows the first frame of each entity. The rectangle of each entity type is written once, in `src/client/Rendering/EntitySpriteConstants.hpp`. The sheet is listed in `titleAssets()`, for the squadron of the title screen, and in `gameAssets()`.
