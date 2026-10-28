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

The assets of the interface, which any screen may use at any time, in a game or outside one, the fonts and every sound, are loaded once at launch and never released. The assets of the intro, those of the title screen (its logo and the entity sheet of its squadron) and the effect sheet of its explosions, are loaded at launch too, so the intro has them from its first frame, but they are not kept: the title releases the effect sheet, and entering a game releases the logo. The assets of a game are loaded by the loading screen when the player enters it: it first releases the loaded assets the game does not list, except the interface ones, then loads the missing ones one file per frame, drawing its progress between two files. An asset already loaded is never read again.

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
| `sprites/hud.png` | The sheet of the status band of a game: the lives of the four player colors, a life lost, the marker of each player, and the icons of the notices of a game: the door of a departure and the bars of a weak signal | Drawn for this project (HUD design system, proposal B; the two icons from the disconnect notices design system); not taken from the sprite set shipped with the subject |
| `sprites/flight_log.png` | The flight log of the end screen: the front cover, the inside of the cover with the left page, the right half with its page, the label of the taunt, a tile of the shadow, the medal of the best pilot and the cross of QUIT | Drawn for this project (end screen design system, proposal A v2), generated from the code of the design with its seeds; the four markers copied from `sprites/hud.png`; not taken from the sprite set shipped with the subject |
| `icons/phosphor/plugs_bold.png` | The unplugged plug of a pilot whose signal is lost, white on a square of 64 × 64 pixels | "plugs", bold weight, of Phosphor Icons, MIT License, copyright 2023 Phosphor Icons, license in `icons/phosphor/LICENSE.txt`; from `phosphor-icons/core` at tag `v2.0.8` (`assets/bold/plugs-bold.svg`), rendered to PNG and made white |
| `sounds/kenney/bydo_growl.ogg` | The growl of the Bydo that opens the intro, 0.50 s | `slime_000.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/enemy_shot.ogg` | The shot of a Bydo, 0.26 s | `laserRetro_002.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/flyby.ogg` | A ship passing at the start of each close-up of the intro, 0.53 s | `doorOpen_001.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/player_shot.ogg` | The shot of a player, 0.24 s | `laserSmall_000.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/small_explosion.ogg` | A Bydo destroyed, 0.78 s | `explosionCrunch_000.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/heavy_impact.ogg` | The first kill of the cascade of the intro, 1.00 s | `lowFrequency_explosion_001.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/entry_whoosh.ogg` | The squadron and the reinforcements entering the intro, 0.95 s | `forceField_000.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/charge.ogg` | The charge of the four ships of the intro, 0.96 s | `forceField_002.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/beam.ogg` | The release of the four beams of the intro, 0.68 s | `laserLarge_000.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/big_explosion.ogg` | A ship destroyed, and the ring of the intro bursting, 1.98 s | `explosionCrunch_004.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/boost.ogg` | The boost of the intro, 0.95 s; the thruster sounds of the pack loop for 5 s | `forceField_004.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/kenney/logo_impact.ogg` | The logo of the intro landing, 0.63 s | `impactMetal_000.ogg` of Kenney's Sci-fi Sounds 1.0, CC0, license in `sounds/kenney/license-sci-fi-sounds.txt`; from kenney.nl |
| `sounds/epidemic/hit.flac` | An entity hit that survives, 0.305 s | The first hit of "Fight, Impact, Hit, Hard, Melon", from Epidemic Sound: the file repeats a hit every 1.6 s, so only the first one is kept, from 5 ms before its attack (0.319 s) to 0.624 s, with a fade out of 30 ms; see [the license of Epidemic Sound](#the-license-of-epidemic-sound) |
| `sounds/kenney/player_hit.ogg` | A player hit, 0.65 s | `impactPunch_heavy_000.ogg` of Kenney's Impact Sounds 1.0, CC0, license in `sounds/kenney/license-impact-sounds.txt`; from kenney.nl |
| `sounds/kenney/click.ogg` | A button or a tab of a menu activated, 0.05 s | `tick_004.ogg` of Kenney's Interface Sounds 1.0, CC0, license in `sounds/kenney/license-interface-sounds.txt`; from kenney.nl |
| `music/oh_the_city.mp3` | The music of a game, Oh the City by Machine Dreams: 160.2 s, which starts at once; it loops from its start to 152 s, before its fade out, which starts at 152.1 s | From Epidemic Sound (downloaded as `ES_Machine Dreams - Oh the City.mp3`); see [the license of Epidemic Sound](#the-license-of-epidemic-sound) |
| `music/spacesuit_suite.mp3` | The theme, Spacesuit Suite by Stationary Sign: 201 s at 109 beats a minute, its first beat 85 ms in; it loops over 352 beats from that beat (to 193.85 s), before its fade out, which starts at 197 s | From Epidemic Sound (downloaded as `ES_Spacesuit Suite - Stationary Sign.mp3`), licensed by the team; see the note on its license below |

## The entity sheet

`sprites/entities.png` is 66 × 140 pixels and holds every sprite at its native size: one pixel of the sheet is one pixel of the background grid, which covers 4 × 4 logical units of the playfield. Each entity holds one row with its two animation frames, 2 pixels apart; rows are 2 pixels apart too, and everything outside the frames is transparent.

| Entity | Native size | Frame time |
|---|---|---|
| Player ship, 4 colors, one row each | 32 × 16 | 80 ms |
| Bydo | 24 × 24 | 250 ms |
| Bonus Bydo | 24 × 24 | 200 ms |
| Player missile | 16 × 6 | 60 ms |
| Enemy missile | 8 × 8 | 120 ms |

The light shade of player 4, `#ffd38b`, is one step of blue above `#ffd38a`, the gold of every flame, here and in `sprites/hud.png`: a palette for color blindness swaps the three shades of a player color, and this step keeps the flame of player 4 out of the swap.

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

`sprites/hud.png` is 88 × 30 pixels, laid out like the entity sheet: native size, frames and rows 2 pixels apart, transparent outside the frames. One pixel of the sheet is one pixel of the interface picture.

| Row | Frames | Frame size | Top |
|---|---|---|---|
| Lives: the ship of each player color, then a life lost, in the shade of the far stars | 5 | 16 × 8 | 0 |
| Markers: triangle, square, circle and star, one per player color, so a player is never told by color alone | 4 | 7 × 7 | 10 |
| Notices: the door a pilot left through, then the bars of a weak signal, pale and dark blue outlined in ink, so no palette recolors them | 2 | 9 × 11, 13 × 10 | 19 |

The rectangles are written once, in `src/client/Rendering/Interface/HudSpriteConstants.hpp`, with the id of the plug icon, which is drawn smoothed at the resolution of the window and tinted, not pixel for pixel. `client_rendering_tests` checks them against the shipped files: every frame lies inside the sheet, shows a visible pixel, cuts no drawing and overlaps no other, and every visible pixel of the icon is white. Both files are listed in `gameAssets()` only.

## The flight log sheet

`sprites/flight_log.png` is 628 × 258 pixels, laid out like the HUD sheet: native size, frames 2 pixels apart, transparent outside the frames. One pixel of the sheet is one pixel of the interface picture. The textures (leather grain, stitches, brass corners, aged paper, coffee ring) are drawn once into the sheet, so a frame of the end screen draws images, not pixels.

| Frame | Rectangle (left, top, width × height) |
|---|---|
| Front cover, with `FLIGHT LOG`, the four markers and `R-TYPE SQUADRON`, without the label | 0, 0, 208 × 206 |
| Inside of the cover, with the edges of the pages and the left page | 210, 0, 208 × 206 |
| Right half, with the edges of the pages and the right page | 420, 0, 208 × 206 |
| Label of the taunt, its printed frame, without text or tape | 0, 208, 156 × 50 |
| Tile of the dithered shadow, ink one pixel in two | 158, 208, 32 × 32 |
| Medal of the best pilot | 192, 208, 9 × 10 |
| Cross of QUIT, white so a tint gives it its color | 203, 208, 7 × 7 |

The rectangles are written once, in `src/client/Rendering/Interface/FlightLogSheetConstants.hpp`. `client_rendering_tests` checks them against the shipped file: every frame lies inside the sheet, shows a visible pixel, cuts no drawing and overlaps no other, and every visible pixel of the cross is white. The sheet is listed in `gameAssets()`, so the end screen opens without loading after a game; the end screen asks for the same list, so it releases nothing the next game needs.

## The license of Epidemic Sound

The theme, the music of a game and the hit come from Epidemic Sound. Its help pages describe the Creator and Pro subscriptions as covering videos and podcasts the subscriber produces, and point to other licenses for a game. Whether the license the team holds covers this game, shared in a public repository, is for the team to confirm; the team does not plan to publish the game.
