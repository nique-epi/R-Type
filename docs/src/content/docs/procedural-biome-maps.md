---
title: Generating the maps of a biome
description: How the procedural generator builds the background maps of a biome, how to run it, and how to create a biome of your own.
---

`generator/procedural/` is a Python tool that draws the background maps of a biome and exports them as an LDtk project. A map is a chain of screens of 480 × 272 pixels, each one a zone template written by hand; the tool draws the terrain, the decor and the light, then keeps the map only if it passes its level design checks. It is a tool of the repository, never shipped with the game, and it ships without any biome: a biome is a folder of data kept outside the repository, and maps are generated locally. The client does not draw these maps yet (see [Decor of a biome](/R-Type/client/#decor-of-a-biome-biomedecor)).

This page tells you how to run it, how a map is built, and how to write a biome of your own.

| Part | State | Where |
|---|---|---|
| Map generator | Available | `generator/procedural/generate_maps.py` |
| Level design checks | Available | `generator/procedural/rules.py` |
| LDtk export and its checker | Available | `generator/procedural/export_ldtk.py`, `tools/verify_ldtk.py` |
| Two level layouts: `corridor` and `field` | Available | `generator/procedural/layouts.py`, `field.py` |
| Loader of a biome | Available | `generator/procedural/biome_loader.py`, `biome_file.py` |
| Starting biome | Available | `generator/procedural/tools/new_biome.py` |
| Biomes | Kept outside the repository, in the folder `--biomes` names | none |
| Palette from pictures, sprite import, preview | Available | `generator/procedural/tools/` |
| LDtk project generated for the Pit: six maps of three screens | Available | `design/fosse-generated-ldtk/` |
| Drawing the maps in the game | Not yet written | none |

## Running it

The tool needs Python 3 with `numpy` and `Pillow`; `jsonschema` only serves the LDtk checker. Every command below runs from `generator/procedural/`. Every command takes `--biomes`, the folder that holds one folder per biome, and `--biome`, the name of one of them; both are required, and the folder lies wherever you like outside the repository.

```bash
python3 -m pip install -r requirements.txt
```

```bash
python3 tools/new_biome.py --biomes ../../../my_biomes hive
```

This writes `my_biomes/hive/biome.json`, a starting biome that runs at once, with no sprite.

```bash
python3 generate_maps.py --biomes ../../../my_biomes --biome hive --count 6 --zones 3 --seed 57 --out out
```

This writes `out/map_57.png` to `out/map_62.png`, one `map_<seed>.json` beside each, and `out/contact.png`, which shows them all with the templates of each. The same seed always gives the same map. When a map fails a check, the tool tries a derived seed, 20 attempts at most, and prints why each attempt was rejected.

```bash
python3 export_ldtk.py --biomes ../../../my_biomes --biome hive --maps out --out ldtk
```

This writes `ldtk/hive_procgen.ldtk` with its atlases. `--sequence a,b*,c` forces the chain of templates of `generate_maps.py` (a trailing `*` mirrors one), and `--layers` also writes the three planes of each map.

A folder of biomes or a biome that does not exist, a `biome.json` that is not valid JSON, and a key every biome must have and this one lacks, each end the command with one line that names the file and what is wrong.

## How long a map is

`--zones` sets the length: a map is 480 × zones pixels wide. A level scrolls at `LEVEL_SCROLL_SPEED` (120 logical units per second, `src/game/Level/LevelConstants.hpp`), and the playfield is 1920 units wide for 480 pixels, so the play plane moves by 30 pixels per second.

| `--zones` | Width | Time to scroll from the first screen to the last |
|---|---|---|
| 3 | 1440 px | 32 s |
| 6 | 2880 px | 80 s |
| 12 | 5760 px | 176 s |

A map never uses a template twice while the biome still has unused ones, except one that carries water, which may come back mirrored to close a basin. Past that count, templates come back, never twice in a row: with eight templates, a map of twelve zones repeats four of them with other heights and other decor.

## How a map is built

1. **Chain.** The generator draws a chain of templates whose connectors match. Each side of a template carries a connector, a name such as `high`, `low` or `water`; two neighbors must carry the same one, and a map never starts or ends on a connector that lets the water through.
2. **Terrain.** Each template gives four profiles as steps: the ground and the ceiling of the play plane and of the second plane. The generator moves the steps by a few pixels, then each material of the biome is painted by its painter.
3. **Decor.** The hero of each zone goes on a third of the screen. Clusters fill the places the template names: a large piece, a medium one touching it, a small one apart. Seams between two materials hide behind a rock and a pier.
4. **Light.** Each plane is lit by the same field: an ambient level per zone, then one pool of light per source, in dithered bands.
5. **Checks.** `rules.py` measures the map. A map that fails one check is never written.

The picture is the stack of three planes: the far plane (flat silhouettes), the second plane and the play plane.

## Layouts: `corridor` and `field`

A template names its layout with `"layout"`. Without it the layout is `corridor`: a floor and a ceiling on two planes, as described above. The `field` layout has neither: its terrain is made of floating bodies, painted with the materials of the biome.

| Plane | In a `field` zone |
|---|---|
| Far | banks of haze, stars in three intensities, small two-tone silhouettes |
| Second | the hero body of the zone, then groups of bodies anywhere |
| Play | only bodies that show past the top of the screen (down to y = 40) or past its bottom (from y = 217) |

Bodies come in groups: a large one, a medium one close to it, a small one apart, then empty space. A silhouette is cut in steps and never drawn from a curve, so no two bodies are alike. Decor stands on a body or hangs under it. The hero of a zone is `great_body`, its largest body with a glowing vein, or `wreck`, a hull built with a material of the biome.

The two layouts do not mix in one map: a chain of templates that mixes them is refused. The keys of a `field` template are listed in the `README.md` of the tool.

## What every biome shares: `frame.py`

`frame.py` holds the corridor the game is played in, in pixels of a 480 × 272 screen. A biome cannot change it.

| Measure | Value |
|---|---|
| Nothing solid on the play plane | from y = 48 to y = 212 (`PLAY_TOP`, `PLAY_BOTTOM`) |
| Lower edge of the ceiling of the play plane | y = 40 at the lowest (`CEIL_LIMIT`) |
| What hangs from that ceiling | down to y = 72 (`HANG_LIMIT`) |
| Props of the play plane | none between y = 96 and y = 176 (`DECOR_CLEAR`) |
| Second plane | ceiling down to y = 64, ground up to y = 196 |
| Status band of the HUD | y ≥ 246 (`HUD_Y`): nothing that matters lies under it |

## The checks

| Check | What it refuses |
|---|---|
| R3 | terrain that reaches no border of the picture, a prop that rests on nothing |
| R4 | a column off the material that bears columns, in the water, or alone |
| R5 | water outside a basin closed by terrain, a bank that drops straight down |
| R6 | a waterfall without a source or a pool, more than one every three zones |
| R7 | a zone where the main material makes less than 60 % of the terrain |
| R8 | anything in the play corridor |
| R9 | a lone prop, clusters closer than 20 pixels, clusters at even spacing |
| R10 | the same sprite twice within 160 pixels on a plane |
| R11 | a zone without one hero on a third of the screen |
| R12 | a dry change of material that nothing hides |
| R13 | more than 6 % of vivid pixels |
| R14 | a zone with no decor on one of the three planes |
| R15 | a zone without a light source, a map whose zones all share one mood |
| R17 | a hero, a light source or foam under the status band |

The checks follow the layout. In a `field` zone, R3 becomes: each body is in one piece, two bodies of a plane neither overlap nor touch, no body of the play plane is cut by a zone edge, and every prop has a body under it. R9 and R10 also cover the bodies: groups of three sizes with empty space between them, and no silhouette twice within 160 pixels. R4, R5, R6 and R12 have no object there and are printed as `n/a`, not as passed.

Two rules are not checked: the terrain is drawn before the decor by construction, and the rule against smooth surfaces and one-pixel outlines is judged by eye. `README.md` of the tool gives the source of each rule.

## What a biome is

A biome is a folder of the folder `--biomes` names: one data file, `biome.json`, and a folder of sprites. The engine holds no name of any biome and no value of any biome: it loads the one `--biome` names.

| Key of `biome.json` | Content |
|---|---|
| `palette` | one letter per color on the play plane (`near`), the second plane (`mid`, or `mid_share`), the backdrop, the two far tones, and named tints of light |
| `materials` | each material: the painter that draws it and its settings, what grows on it, what hides its seams |
| `elements` | water, waterfall band, lamps, column, small strokes, floating specks; stars, haze and marks for a `field` |
| `decor` | only what file names cannot say; every key is optional |
| `templates` | the zones: material, connectors, hero, mood, then the profiles or the groups of bodies of their layout |
| `drawn`, `sheets` | sprites drawn in palette letters, and sheets to cut (optional) |

Beside it, `sprites/` holds the sprites, in the exact colors of `palette.near`. Every other key refers to colors by their letter. That is why two biomes with the same letters can share sprites: a biome whose `decor.sprites_from` names another biome of the same folder reads its sprites letter for letter and shows them in its own colors. A string that starts with `@`, such as `"mood": "@WARM"`, stands for the tint of that name. The `README.md` of the tool describes every key.

A biome picks what draws it among registered names:

| Kind | Names |
|---|---|
| Layouts | `corridor`, `field` |
| Painters | `cobble` (packed stones), `soil` (dark earth), `bricks` (courses of blocks), `plates` (riveted plates) |
| Heroes | `colonnade`, `cluster`, `waterfall`, `light_well`; in a `field`: `great_body`, `wreck` |
| Body silhouettes | `boulder`, `hull` |
| Prop shapes | `pile`, `wall`, `steps`, `slab`, `pillar`, `pillar_broken`, `drum`, `lintel` |

`bricks` is tuned by its settings (`cell`, `span`, `bond`, `bevel`, `chips`, `tints`), so two biomes can have masonry that looks nothing alike without new code.

## Creating a biome

The example makes a biome named `hive` in `my_biomes/`, from the pictures of an existing background.

1. **Write a starting biome.** `new_biome.py` writes a palette, one material that builds its own props by rule, and two templates. It passes every check as it is. `--layout field` starts a biome of open space instead.

```bash
python3 tools/new_biome.py --biomes ../../../my_biomes hive --layout corridor
```

2. **Take the colors from pictures.** `make_palette.py` finds the color ramps of the pictures and fills the standard letters in `biome.json`, so the rest of the starting biome still works.

```bash
python3 tools/make_palette.py --biomes ../../../my_biomes --biome hive ../../assets/backgrounds/ember_reach_near.png ../../assets/backgrounds/ember_reach_middle.png
```

It prints the ramps it found and the role each one got (`stone`, `rock`, `plant`, `liquid`, `stem`, `accent`); `--assign stone=1,liquid=0` overrules the choice. The second plane is each color halfway to the backdrop. The tints of the old palette are kept.

3. **Look at the materials and the props they build.**

```bash
python3 tools/preview_biome.py --biomes ../../../my_biomes --biome hive --out preview.png
```

A material with `"props": true` builds its own props by rule, named `gen:<material>:<shape>`: a wall, a pillar, steps, a pile. They are painted by the painter of the material, so changing the material changes the props, with no sprite at all.

4. **Bring your own sprites.** Pictures with a transparent background, one sprite per file, or a sheet cut by its empty gutters:

```bash
python3 tools/import_sprites.py --biomes ../../../my_biomes --biome hive path/to/pictures
```

```bash
python3 tools/import_sprites.py --biomes ../../../my_biomes --biome hive sheet.png --grid 4x4 --names a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p
```

The tool crops each sprite and moves each pixel to the nearest color of the palette. Name a file `<family>.<size>.<name>[.glow][.hang][.hero].png` and the sprite needs no line of `decor`: `size` is `large`, `medium` or `small`; `glow` makes it a light; `hang` keeps it under ceilings; `hero` keeps it for the hero of a zone.

5. **Write the templates.** In a corridor, each one names a material, two connectors, the four profiles as `[x, y]` steps, a hero at x = 160 or x = 320, and the families of its clusters; a template with `"sky": true` has no ceiling. In a field, each one names its hero body and the places of its groups of bodies.

6. **Generate, look, export.**

```bash
python3 generate_maps.py --biomes ../../../my_biomes --biome hive --count 3 --zones 6 --seed 1 --out out_hive
```

```bash
python3 export_ldtk.py --biomes ../../../my_biomes --biome hive --maps out_hive --out ldtk_hive
```

Give each biome its own output folders: `props_16.png` has the same name for all. A biome and its maps stay local; when a biome is ready, the LDtk project exported from it and its atlases can be committed where the game reads them.

## LDtk export

The project follows the JSON format of LDtk 1.5.3: one level per map, named `<Biome>_<seed>`, on a 16 pixel grid. Its layers, from top to bottom, are `Mobs` (entities, empty), `Walls` (an IntGrid whose value 1 marks a cell at least half solid on the play plane, hidden when the project opens) and `Near`, `Mid`, `Far` (tiles, one atlas per plane). The light is baked in the tiles, so the atlases give the picture back but are not meant to paint with; `props_16.png` holds the unlit sprites for that.

```bash
python3 tools/verify_ldtk.py --biomes ../../../my_biomes --biome hive --maps out --project ldtk --schema JSON_SCHEMA.json
```

The checker validates the project against the official schema (download it from <https://ldtk.io/files/JSON_SCHEMA.json>), rebuilds each level from its tiles and compares it with the map pixel by pixel, and checks that no `Walls` cell lies in the play corridor.

## What is not supported

- A map that mixes `field` and `corridor` zones; in a `field` zone, water, waterfalls, columns, hanging lamps and bodies across the play corridor.
- Looping parallax layers such as the ones in `assets/backgrounds/`: this tool makes fixed maps.
- A drawn sky: under `"sky": true` the backdrop is one flat color.
- Platforms across the play corridor, and more than two planes of terrain.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change the corridor kept clear for the game | `generator/procedural/frame.py` |
| Add or tune a check | `generator/procedural/rules.py` |
| Add a material unlike the four painters | a function under `@register(PAINTERS, …)` in `painters.py` |
| Add a kind of hero | a function under `@register(HEROES, …)` in `heroes.py` |
| Add a shape of generated prop | a function under `@register(SHAPES, …)` in `props.py` |
| Add a level layout | a function under `@register(LAYOUTS, …)` and its list in `CHECKS` of `rules.py` |
| Add a zone to a biome | `templates` in the `biome.json` of the biome |
| Change what a starting biome holds | `generator/procedural/tools/new_biome.py` |
| Change what LDtk receives | `generator/procedural/export_ldtk.py` |
