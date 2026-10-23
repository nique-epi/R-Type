# Assets

Every file the client reads at run time.

| Folder | Holds |
|---|---|
| `sprites/` | textures and sprite sheets |
| `sounds/` | short sound effects, loaded in memory |
| `music/` | music, read from disk while it plays |
| `fonts/` | fonts |
| `icons/` | icons drawn smooth, one folder per icon set with its license |

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
| `sprites/effects.png` | The effects of a match: the explosions of the Bydo and of the ship, the impact of a missile and the muzzle flashes of both missiles, each with its version without flash | Drawn for this project (animation proposal, options C), generated from the drafts of the proposal; not taken from the sprite set shipped with the subject |
| `sprites/hud.png` | The sheet of the status band of a game: the lives of the four player colors, a life lost, and the marker of each player | Drawn for this project (HUD design system, proposal B); not taken from the sprite set shipped with the subject |
| `icons/phosphor/plugs_bold.png` | The unplugged plug of a pilot whose signal is lost, white on a square of 64 × 64 pixels | "plugs", bold weight, of Phosphor Icons, MIT License, copyright 2023 Phosphor Icons, license in `icons/phosphor/LICENSE.txt`; from `phosphor-icons/core` at tag `v2.0.8` (`assets/bold/plugs-bold.svg`), rendered to PNG and made white |

## The entity sheet

`sprites/entities.png` is 66 × 140 pixels and holds every sprite at its native size: one pixel of the sheet is one pixel of the background grid, which covers 4 × 4 logical units of the playfield. Each entity holds one row with its two animation frames, 2 pixels apart; rows are 2 pixels apart too, and everything outside the frames is transparent.

| Entity | Native size | Frame time |
|---|---|---|
| Player ship, 4 colors, one row each | 32 × 16 | 80 ms |
| Bydo | 24 × 24 | 250 ms |
| Bonus Bydo | 24 × 24 | 200 ms |
| Player missile | 16 × 6 | 60 ms |
| Enemy missile | 8 × 8 | 120 ms |

The game and the squadron of the title screen show the two frames in turn, each for the frame time above. The rectangle, the frames and the animation of each entity type are written once, in `src/client/Rendering/Sprites/EntitySpriteConstants.hpp`. The sheet is listed in `titleAssets()`, for the squadron of the title screen, and in `gameAssets()`.

## The effect sheet

`sprites/effects.png` is 418 × 234 pixels, laid out like the entity sheet: native size, one row per effect, frames and rows 2 pixels apart, transparent outside the frames. Each effect has two rows of as many frames: the standard one, then the one played when the player turns REDUCE FLASHES on, which shows no white.

| Effect | Frames | Frame size | Frame time | Top of the standard row, then of the row without flash |
|---|---|---|---|---|
| Bydo explosion: the Bydo turns lilac, becomes a blob, then bursts into purple drops | 6 | 48 × 48 | 70 ms | 0, 50 |
| Ship explosion: three fireballs, each starting 120 ms after the previous one | 10 | 40 × 36 | 60 ms | 100, 138 |
| Impact of a missile | 3 | 10 × 10 | 50 ms | 176, 188 |
| Muzzle flash of a player missile, cyan | 2 | 7 × 7 | 50 ms | 200, 209 |
| Muzzle flash of an enemy missile, orange | 2 | 7 × 7 | 50 ms | 218, 227 |

Without flash, the Bydo explosion starts on the Bydo in its own colors instead of its lilac silhouette, the fireballs and the impact turn their white to yellow, and a muzzle flash shows its smaller frame twice. The sheet was generated from the drafts of the animation proposal, the same drawings at the same size; the ship explosion lays its three fireballs on each frame, so the client plays one animation for it. The rows are written once, in `src/client/Rendering/Effects/EffectConstants.hpp`, each computed under the previous one, and `client_animation_tests` checks them against the shipped sheet: every frame lies inside it, shows a visible pixel, cuts no drawing and overlaps no other, and no frame without flash shows the white of the explosions (`#fffbe8`). The sheet is listed in `gameAssets()` only.

## The HUD sheet

`sprites/hud.png` is 88 × 17 pixels, laid out like the entity sheet: native size, frames and rows 2 pixels apart, transparent outside the frames. One pixel of the sheet is one pixel of the interface picture.

| Row | Frames | Frame size | Top |
|---|---|---|---|
| Lives: the ship of each player color, then a life lost, in the shade of the far stars | 5 | 16 × 8 | 0 |
| Markers: triangle, square, circle and star, one per player color, so a player is never told by color alone | 4 | 7 × 7 | 10 |

The rectangles are written once, in `src/client/Rendering/Interface/HudSpriteConstants.hpp`, with the id of the plug icon, which is drawn smoothed at the resolution of the window and tinted, not pixel for pixel. `client_rendering_tests` checks them against the shipped files: every frame lies inside the sheet, shows a visible pixel, cuts no drawing and overlaps no other, and every visible pixel of the icon is white. Both files are listed in `gameAssets()` only.
