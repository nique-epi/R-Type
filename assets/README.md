# Assets

Every file the client reads at run time, and the source of the dictionary of the protocol.

| Folder | Holds |
|---|---|
| `sprites/` | textures and sprite sheets |
| `backgrounds/` | the layers of the decor of each biome, one picture per layer |
| `sounds/` | short sound effects, loaded in memory |
| `music/` | music, read from disk while it plays |
| `fonts/` | fonts |
| `icons/` | icons drawn smooth, one folder per icon set with its license |
| `entities/` | one TOML sheet per entity type: its gameplay, read by the server, and its appearance, read by the client (see [Writing an entity sheet](https://nique-epi.github.io/R-Type/entity-sheets/)) |

## Asset ids

Code names a file by its asset id: its path in this folder, with `/` between folders, for example `sprites/player_ship.png`. Ids are written once, as named constants.

Every folder and file name of an id starts with a lowercase letter `a-z` or a digit, then holds only `a-z`, digits, `_`, `-` and `.`. The same id then names the same file on Linux, macOS and Windows, and never names a file outside this folder.

## When files are loaded

The assets of the interface, which any screen may use at any time, in a game or outside one, the fonts and every sound, are loaded once at launch and never released. The assets of the intro, those of the title screen (its logo, the entity sheet of its squadron, and the HUD sheet and the professor sheet the help of the waiting room draws, since the waiting room is pushed over the title) and the effect sheet of its explosions, are loaded at launch too, so the intro has them from its first frame, but they are not kept: the title releases the effect sheet, and entering a game releases the logo. The assets of a game are loaded by the loading screen when the player enters it: it first releases the loaded assets the game does not list, except the interface ones, then loads the missing ones one file per frame, drawing its progress between two files. An asset already loaded is never read again.

When an id breaks the rule, or its file is missing or cannot be read, the client stops once every file of the list was tried, and names every one of them. Music is only located: its file is read while it plays.

## How the programs find this folder

The client and the server look for `assets/` next to their executable, then in the folder above it, where the Visual Studio generator writes `Release/` and `Debug/`. The folder they were launched from does not matter. The server reads only `waves/waves.toml` and the sheets of `entities/` from it.

## Not read at run time

`compression/server_to_client.dict` is the zstd dictionary of the protocol. CMake compiles it into the client and the server, and the packages do not hold it.

## Where the files come from

| File | What it holds | Source |
|---|---|---|
| `fonts/press_start_2p/press_start_2p.ttf` | Press Start 2P, the pixel font of the menus: bars, headings, lines of text | SIL Open Font License 1.1, copyright 2012 The Press Start 2P Project Authors, license in `fonts/press_start_2p/OFL.txt`; from `google/fonts` (`ofl/pressstart2p`) |
| `fonts/silkscreen/silkscreen.ttf` | Silkscreen, the small pixel font of the menus: version, copyright | SIL Open Font License 1.1, copyright 2001 The Silkscreen Project Authors, license in `fonts/silkscreen/OFL.txt`; from `google/fonts` (`ofl/silkscreen`) |
| `sprites/logo.png` | The logo of the title screen, 210 × 42 pixels | Drawn for this project (main menu mockup 1c v4) |
| `sprites/entities.png` | The sprite sheet of a match: four player ships, the Bydo, the bonus Bydo, the player missile and the enemy missile | Drawn for this project (proposal 3a); not taken from the sprite set shipped with the subject |
| `sprites/mobs/voltjelly.png` | The Voltjelly: a Bydo jellyfish inside an electric aura that turns, one row of 4 frames; and its death | Drawn for this project (design system "R-Type — new mobs"), exported by the code of the design; not taken from the sprite set shipped with the subject |
| `sprites/mobs/reaper.png` | The Reaper: a Bydo skeleton with a scythe, idle, raising its scythe, swinging it, open; its bone, which no sheet uses yet; and its death | Drawn for this project (design system "R-Type — new mobs"), exported by the code of the design, the white of the blade turned to its light tint; not taken from the sprite set shipped with the subject |
| `sprites/mobs/drillback.png` | The Drillback: an armoured Bydo beetle with a drill for a horn, idle, shaking, charging, stunned; and its death | Drawn for this project (design system "R-Type — new mobs"), exported by the code of the design, the white of the drill and of the flame turned to their light tint, the two stunned frames drawn twice; not taken from the sprite set shipped with the subject |
| `sprites/mobs/bydomancer.png` | The Bydomancer: a Bydo mage with a staff, idle, raising its staff, casting, resting; its rune orb, the shard of the orb, its nova, a green ring around its body; and its death | Drawn for this project (design system "R-Type — new mobs"), exported by the code of the design, the idle body drawn at the center of each frame of the nova; not taken from the sprite set shipped with the subject |
| `sprites/mobs/bydo_tyrant.png` | The Bydo Tyrant, the final boss: a giant Bydo with a skull, idle, its mouth open, its burst, its eye charging, its beam, its brood sacs, its tentacles, its rage, then the same states in its enraged skin; and its spore mine | Drawn for this project (boss design system), generated from the code of the design: the 64 × 64 frame of the mobs design scaled by Scale2x to 128 × 128, with one-pixel detail and an enraged skin; not taken from the sprite set shipped with the subject |
| `sprites/effects.png` | The effects of a match: the explosions of the Bydo and of the ship, the impact of a missile and the muzzle flashes of both missiles, each with its version without flash | Drawn for this project (animation proposal, options C), generated from the drafts of the proposal; not taken from the sprite set shipped with the subject |
| `sprites/hud.png` | The sheet of the status band of a game: the lives of the four player colors, a life lost, the marker of each player, and the icons of the notices of a game: the door of a departure and the bars of a weak signal | Drawn for this project (HUD design system, proposal B; the two icons from the disconnect notices design system); not taken from the sprite set shipped with the subject |
| `sprites/professor.png` | Professor Bydo, the guide of the help of the waiting room: the Bydo under a mortarboard, round glasses over its eye, two frames facing left, then the same two mirrored | Drawn for this project (help design system, version 2), from the Bydo of `sprites/entities.png`; not taken from the sprite set shipped with the subject |
| `sprites/looks.png` | The items the ship wears in the hangar and the look items floating in a game: eight hats, shades and a mustache, one cell of 16 × 16 each | Drawn for this project (design system "R-Type Hangar"); not taken from the sprite set shipped with the subject |
| `sprites/leaderboard.png` | The sheet of the leaderboard: 32 frames of a banner in the wind, 52 × 99 pixels, then the guard of the court, 28 × 30, two frames, then mirrored | The silhouette and the weave of the banner come from the left red banner of `castle_tileset_part3.png`, [pixel art castle tileset](https://opengameart.org/content/pixel-art-castle-tileset) by rubberduck, CC0, recolored; its folds, trim, tassels, rod and emblem, and the guard, were drawn for this project |
| `sprites/sector_map.png` | The sheet of the map of the servers: a blue planet that turns, in three sizes and ringed in orange, a dark planet that stands still, and the golden house of the hangar | Drawn for this project (design "Death Frames and Map Icons"), generated by a script; not taken from the sprite set shipped with the subject |
| `backgrounds/<biome>_haze.png`, `_far.png`, `_middle.png`, `_near.png` | The four layers of the decor of the biomes `drift_fields`, `ember_reach` and `hive_depths`, 960 × 270 pixels each, repeated side by side without a seam: a dithered haze of nebulae, far bodies, middle rocks, and near silhouettes along the top and the bottom, in the dark blues, embers and greens of each biome | Drawn for this project by `tests/tools/biome-layers/make_biome_layers.py`, from seeded draws |
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
| `sounds/boss_alarm.mp3` | The alarm of the Bydo Tyrant arriving, 8.23 s; the music of the boss starts when it ends | Provided by the team; source and license to be stated |
| `sounds/robot_arm.flac` | The robot arm of the hangar fitting an item, 1.16 s | Provided by the team (`son bras.mp3`, peak at -17 dBFS), cut 0.13 s of leading silence and raised by 16.2 dB to a peak of -1 dBFS like the other sounds; source and license to be stated |
| `sounds/kenney/click.ogg` | A button or a tab of a menu activated, 0.05 s | `tick_004.ogg` of Kenney's Interface Sounds 1.0, CC0, license in `sounds/kenney/license-interface-sounds.txt`; from kenney.nl |
| `music/oh_the_city.mp3` | The music of a game, Oh the City by Machine Dreams: 160.2 s, which starts at once; it loops from its start to 152 s, before its fade out, which starts at 152.1 s | From Epidemic Sound (downloaded as `ES_Machine Dreams - Oh the City.mp3`); see [the license of Epidemic Sound](#the-license-of-epidemic-sound) |
| `music/boss_theme.mp3` | The music of the Bydo Tyrant, 192 s, played in a loop from the end of its alarm until the game ends | Provided by the team; source and license to be stated |
| `music/space_ambience.mp3` | The ambience of the server map, 37.7 s, in a loop under the theme at 0.25 of the MUSIC volume | Provided by the team; source and license to be stated |
| `music/hangar_ambience.mp3` | The ambience of the hangar, a garage, 140.7 s, in a loop under the theme at 0.40 of the MUSIC volume | Provided by the team; source and license to be stated |
| `music/hangar_crowd.flac` | People talking in the hangar, 180.5 s, in a loop under the theme at 0.50 of the MUSIC volume, with the garage | From Epidemic Sound ("Ambience, Public Place, Shopping Centre, Crowded, Reverberant, Distant Voices, Distant Music, Bangkok 02 (AmbiX B)"), mixed down to mono at 32 kHz and raised by 9 dB, its level being 14 dB under the garage; see [the license of Epidemic Sound](#the-license-of-epidemic-sound) |
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

The game and the squadron of the title screen show the two frames in turn, each for the frame time above. In the game, the frames and the frame time of each entity type are written once, in the `[appearance]` of its sheet in `entities/`; `src/client/Rendering/Sprites/EntitySpriteConstants.hpp` holds those the title, the intro and the waiting room draw, and the bonus Bydo, which no sheet uses yet. The client tests check that the frame times of the shipped sheets are those of the table above. The sheet is listed in `titleAssets()`, for the squadron of the title screen, and in `gameAssets()`.

## The mob sheets

Each mob of `sprites/mobs/`, and the Bydo Tyrant, has a sheet of its own, laid out like the entity sheet: native size, one row per state, frames and rows 2 pixels apart, transparent outside the frames. A frame wider than the body of the mob (an attack that reaches out) is drawn with the `anchor_x` of its `[appearance]` table, so the body stays where the server puts it.

| Sheet | Size | Rows: frames × frame size, frame time |
|---|---|---|
| `bydo_tyrant.png` | 518 × 1959 | one row per state, 130 pixels apart, every frame 128 × 128: idle: 2, 300 ms; mouth open (the roar, the burst signal): 2; burst: 1; eye charging: 2; beam: 1; brood sacs: 2; tentacles: 1; rage: 4, idle and rage in turn, 175 ms; then, in the enraged skin, idle: 2, 200 ms; mouth: 2; burst: 1; eye: 2; beam: 1; brood: 2; tentacles: 1; spore mine, under them at top 1950: 2 × 9 × 9, 200 ms. The frame times of an attack are those of its `[appearance]` table |
| `bydomancer.png` | 298 × 263 | idle: 2 × 24 × 24, 250 ms; staff raised: 2 × 24 × 24, 150 ms; cast: 1 × 30 × 24, 120 ms, the body in the 24 pixels on the right; resting: 1 × 24 × 24, 300 ms; rune orb: 2 × 9 × 9, 120 ms; shard: 1 × 5 × 5, 100 ms; nova: 3 × 41 × 41, 80 ms, the body at the center; death at top 165, then without flash at top 215 |
| `drillback.png` | 298 × 202 | idle: 2 × 24 × 24, 200 ms; shaking: 2 × 24 × 24, 60 ms; charge: 2 × 40 × 24, 60 ms, the body in the 24 pixels on the left; stunned: 4 × 24 × 24, 225 ms; death at top 104, then without flash at top 154 |
| `reaper.png` | 298 × 217 | idle: 2 × 24 × 24, 250 ms; scythe raised: 2 × 24 × 24, 200 ms; swing: 2 × 40 × 24, 70 ms, the body in the 24 pixels on the right; open: 1 × 24 × 24, 600 ms; bone: 4 × 13 × 13, 80 ms; death at top 119, then without flash at top 169 |
| `voltjelly.png` | 298 × 140 | aura: 4 × 40 × 40, 120 ms; the bell, 24 × 24, at the center of each frame; death at top 42, then without flash at top 92 |

The client tests check that the frame times of the idle rows are those of the table above, but for the Tyrant, whose times no test pins yet, and for its diver, `tyrant_diver`, which draws the idle row of `drillback.png` at 120 ms instead of 200. The game screen loads every sheet an `[appearance]` names. The missiles of the Tyrant and the shards of its spores use the enemy missile of `sprites/entities.png`.

A death is 6 frames of 48 × 48 pixels, 70 ms each, laid out like the Bydo explosion of the effect sheet, its row without flash 2 pixels under it. It was generated from the idle frame of its mob: the first frame flashes the silhouette in the lightest shade of the mob, then the body breaks into pieces in its own colors (the Reaper drops its skull, ribs and scythe, the Drillback splits its shell and loses its drill, the Bydomancer's orb bursts into runes while its robe crumples, the Voltjelly's aura shorts out and its bell bursts), which fly apart and fade. Without flash, the first frame shows the mob in its own colors instead. The Voltjelly's frame of 40 × 40 is centered in the 48 × 48. `client_rendering_tests` checks that every death frame lies inside its sheet, shows a visible pixel and cuts no sprite.

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

The rectangles are written once, in `src/client/Rendering/Interface/HudSpriteConstants.hpp`, with the id of the plug icon, which is drawn smoothed at the resolution of the window and tinted, not pixel for pixel. `client_rendering_tests` checks them against the shipped files: every frame lies inside the sheet, shows a visible pixel, cuts no drawing and overlaps no other, and every visible pixel of the icon is white. Both files are listed in `gameAssets()`; the HUD sheet is also listed in `titleAssets()`, for the status band the help of the waiting room paints.

## The professor sheet

`sprites/professor.png` is 50 × 58 pixels, laid out like the entity sheet: native size, frames and rows 2 pixels apart, transparent outside the frames. The help of the waiting room draws it twice its size.

| Row | Frames | Frame size | Top |
|---|---|---|---|
| Professor Bydo facing left, as the entity sheet draws the Bydo | 2 | 24 × 28 | 0 |
| The same frames mirrored, facing right, since the interface canvas cannot mirror an image | 2 | 24 × 28 | 30 |

Each row shows its two frames in turn, 250 ms each, as the Bydo. The rectangles and the animations are written once, in `src/client/Rendering/Interface/ProfessorSpriteConstants.hpp`. `client_rendering_tests` checks them against the shipped file: every frame lies inside the sheet, shows a visible pixel and cuts no drawing, and the second row is the first mirrored. The sheet is listed in `titleAssets()` and `gameAssets()`.

## The look sheet

`sprites/looks.png` is 178 × 16 pixels: ten cells of 16 × 16 in a row, 2 pixels apart, in the order of `SHIP_LOOK_ITEM_NAMES`, each drawing against the top-left corner of its cell, transparent elsewhere. The hangar and the waiting room draw an item at the scale of the ship it sits on.

The size of each drawing and where it sits on the ship are written once, in `src/client/Rendering/Interface/ShipLookSpriteConstants.hpp`. `client_rendering_tests` checks each drawing against the shipped file: it lies inside the sheet, shows a visible pixel and cuts no drawing. The sheet is listed in `titleAssets()` and `gameAssets()`.

## The leaderboard sheet

`sprites/leaderboard.png` is 1726 × 163 pixels: a row of 32 banners of 52 × 99 pixels at the top, 2 pixels apart, a frame every 50 ms; under them, 2 pixels lower, the guard of 28 × 30 pixels, two frames 2 pixels apart, a frame every 250 ms; then the same two mirrored, as the interface canvas cannot mirror an image. The last banner leads to the first without a jump.

The rectangles and the animations are written once, in `src/client/Rendering/Interface/LeaderboardSpriteConstants.hpp`. `client_rendering_tests` checks each frame against the sheet, the mirror of the guards, and that the loop of the banner closes without a jump.

## The sector map sheet

`sprites/sector_map.png` is 86 × 56 pixels, native size, one glyph a row, frames and rows 2 pixels apart: a planet of 5 × 5 pixels at the top, one of 7 × 7 at top 7, one of 9 × 9 at top 16, the slow one of 9 × 9 in an orange ring at top 27, each 8 frames of a light band running across it, a frame every 150 ms; then the planet of another version, 7 × 7 at top 38, one frame; then the house of the hangar, 9 × 9 at top 47. One pixel of the sheet is one pixel of the interface picture.

The rectangles are written once, in `src/client/Widgets/SectorMap/SectorMapSpriteConstants.hpp`. `client_rendering_tests` checks each frame against the sheet. The sheet is listed in `titleAssets()`.

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
