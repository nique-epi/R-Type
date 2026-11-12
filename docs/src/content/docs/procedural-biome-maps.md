---
title: Generating the maps of a biome
description: How the procedural generator builds the background maps of a biome, how to run it, and how to create a biome of your own.
---

`generator/procedural/` is a Python tool that draws the background maps of a biome and exports them as an LDtk project. A map is a chain of screens of 480 × 272 pixels, each one a zone template written by hand; the tool draws the terrain, the decor and the light, then keeps the map only if it passes its level design checks. It is a tool of the repository, never shipped with the game: the client does not draw these maps yet (see [Decor of a biome](/R-Type/client/#decor-of-a-biome-biomedecor)).

This page tells you how to run it, how a map is built, and how to make a biome that looks nothing like the ones provided.

| Part | State | Where |
|---|---|---|
| Map generator | Available | `generator/procedural/generate_maps.py` |
| Level design checks | Available | `generator/procedural/rules.py` |
| LDtk export and its checker | Available | `generator/procedural/export_ldtk.py`, `tools/verify_ldtk.py` |
| Biome `fosse` (the Pit) | Available | `generator/procedural/biomes/fosse/` |
| Biome `demo_ember` (demonstration) | Available | `generator/procedural/biomes/demo_ember/` |
| Palette from pictures, sprite import, preview | Available | `generator/procedural/tools/` |
| Drawing the maps in the game | Not yet written | none |

## Running it

The tool needs Python 3 with `numpy` and `Pillow`; `jsonschema` only serves the LDtk checker. Every command below runs from `generator/procedural/`.

```bash
python3 -m pip install -r requirements.txt
```

```bash
python3 generate_maps.py --biome fosse --count 6 --zones 3 --seed 57 --out out
```

This writes `out/map_57.png` to `out/map_62.png`, one `map_<seed>.json` beside each, and `out/contact.png`, which shows them all with the templates of each. The same seed always gives the same map. When a map fails a check, the tool tries a derived seed, 20 attempts at most, and prints why each attempt was rejected.

```bash
python3 export_ldtk.py --biome fosse --maps out --out ldtk
```

This writes `ldtk/fosse_procgen.ldtk` with its atlases. `--sequence a,b*,c` forces the chain of templates of `generate_maps.py` (a trailing `*` mirrors one), and `--layers` also writes the three planes of each map.

## How long a map is

`--zones` sets the length: a map is 480 × zones pixels wide. A level scrolls at `LEVEL_SCROLL_SPEED` (120 logical units per second, `src/game/Level/LevelConstants.hpp`), and the playfield is 1920 units wide for 480 pixels, so the play plane moves by 30 pixels per second.

| `--zones` | Width | Time to scroll from the first screen to the last |
|---|---|---|
| 3 | 1440 px | 32 s |
| 6 | 2880 px | 80 s |
| 12 | 5760 px | 176 s |

A map never uses a template twice while the biome still has unused ones, except one that carries water, which may come back mirrored to close a basin. Past that count, templates come back, never twice in a row: `fosse` has eight, so a map of twelve zones repeats four of them with other heights and other decor.

## How a map is built

1. **Chain.** The generator draws a chain of templates whose connectors match. Each side of a template carries a connector, a name such as `high`, `low` or `water`; two neighbors must carry the same one, and a map never starts or ends on a connector that lets the water through.
2. **Terrain.** Each template gives four profiles as steps: the ground and the ceiling of the play plane and of the second plane. The generator moves the steps by a few pixels, then each material of the biome is painted by its painter.
3. **Decor.** The hero of each zone goes on a third of the screen. Clusters fill the places the template names: a large piece, a medium one touching it, a small one apart. Seams between two materials hide behind a rock and a pier.
4. **Light.** Each plane is lit by the same field: an ambient level per zone, then one pool of light per source, in dithered bands.
5. **Checks.** `rules.py` measures the map. A map that fails one check is never written.

The picture is the stack of three planes: the far plane (flat silhouettes), the second plane and the play plane.

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

Two rules are not checked: the terrain is drawn before the decor by construction, and the rule against smooth surfaces and one-pixel outlines is judged by eye. `README.md` of the tool gives the source of each rule.

## What a biome is

A biome is a folder of `generator/procedural/biomes/`. The engine holds no name of any biome: it loads the one `--biome` names.

| File | Content |
|---|---|
| `palette.py` | one letter per color, on the play plane (`NEAR`) and the second plane (`MID`), the backdrop and the two far tones |
| `materials.py` | each material: the painter that draws it and its settings, what grows on it, what hides its seams |
| `elements.py` | water, waterfall band, lamps, column, small strokes, floating specks |
| `decor.py` | only what file names cannot say; every name is optional |
| `templates.py` | the zones: material, connectors, four profiles, hero, places of the clusters, mood |
| `sprites/` | the sprites, in the exact colors of `NEAR` |

Every other file of a biome refers to colors by their letter. That is why two biomes with the same letters can share sprites: `demo_ember` reads the sprites of `fosse` letter for letter and shows them in its own colors.

A biome picks what draws it among registered names:

| Kind | Names |
|---|---|
| Painters | `cobble` (packed stones), `soil` (dark earth), `bricks` (courses of blocks), `plates` (riveted plates) |
| Heroes | `colonnade`, `cluster`, `waterfall`, `light_well` |
| Prop shapes | `pile`, `wall`, `steps`, `slab`, `pillar`, `pillar_broken`, `drum`, `lintel` |

`bricks` is tuned by its settings (`cell`, `span`, `bond`, `bevel`, `chips`, `tints`), so two biomes can have masonry that looks nothing alike without new code.

## Creating a biome

The example makes a biome named `hive` from the pictures of an existing background.

1. **Copy the demonstration biome.** It is the smallest complete one.

```bash
cp -R biomes/demo_ember biomes/hive
```

2. **Take the colors from pictures.** `make_palette.py` finds the color ramps of the pictures and fills the standard letters, so every file copied at step 1 still works.

```bash
python3 tools/make_palette.py --biome hive ../../assets/backgrounds/ember_reach_near.png ../../assets/backgrounds/ember_reach_middle.png
```

It prints the ramps it found and the role each one got (`stone`, `rock`, `plant`, `liquid`, `stem`, `accent`); `--assign stone=1,liquid=0` overrules the choice. The second plane is each color halfway to the backdrop, the share measured on the palette of `fosse`. The tints of the lights the old file held are kept.

3. **Look at the materials and the props they build.**

```bash
python3 tools/preview_biome.py --biome hive --out preview.png
```

A material with `"props": True` builds its own props by rule, named `gen:<material>:<shape>`: a wall, a pillar, steps, a pile. They are painted by the painter of the material, so changing the material changes the props, with no sprite at all.

4. **Bring your own sprites.** Pictures with a transparent background, one sprite per file, or a sheet cut by its empty gutters:

```bash
python3 tools/import_sprites.py --biome hive path/to/pictures
```

```bash
python3 tools/import_sprites.py --biome hive sheet.png --grid 4x4 --names a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p
```

The tool crops each sprite and moves each pixel to the nearest color of the palette. Name a file `<family>.<size>.<name>[.glow][.hang][.hero].png` and the sprite needs no line of `decor.py`: `size` is `large`, `medium` or `small`; `glow` makes it a light; `hang` keeps it under ceilings; `hero` keeps it for the hero of a zone.

5. **Write the templates.** Each one names a material, two connectors, the four profiles as `(x, y)` steps, a hero at x = 160 or x = 320, and the families of its clusters. A template with `"sky": True` has no ceiling.

6. **Generate, look, export.**

```bash
python3 generate_maps.py --biome hive --count 3 --zones 6 --seed 1 --out out_hive
```

```bash
python3 export_ldtk.py --biome hive --maps out_hive --out ldtk_hive
```

Give each biome its own output folders: `props_16.png` has the same name for all.

## LDtk export

The project follows the JSON format of LDtk 1.5.3: one level per map, named `<Biome>_<seed>`, on a 16 pixel grid. Its layers, from top to bottom, are `Mobs` (entities, empty), `Walls` (an IntGrid whose value 1 marks a cell at least half solid on the play plane, hidden when the project opens) and `Near`, `Mid`, `Far` (tiles, one atlas per plane). The light is baked in the tiles, so the atlases give the picture back but are not meant to paint with; `props_16.png` holds the unlit sprites for that.

```bash
python3 tools/verify_ldtk.py --biome fosse --maps out --project ldtk --schema JSON_SCHEMA.json
```

The checker validates the project against the official schema (download it from <https://ldtk.io/files/JSON_SCHEMA.json>), rebuilds each level from its tiles and compares it with the map pixel by pixel, and checks that no `Walls` cell lies in the play corridor.

## What is not supported

- A biome without ground: the terrain, the decor and the checks assume ground under every column.
- Looping parallax layers such as the ones in `assets/backgrounds/`: this tool makes fixed maps.
- A drawn sky: under `"sky": True` the backdrop is one flat color.
- Platforms across the play corridor, and more than two planes of terrain.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change the corridor kept clear for the game | `generator/procedural/frame.py` |
| Add or tune a check | `generator/procedural/rules.py` |
| Add a material unlike the four painters | a function under `@register(PAINTERS, …)` in `painters.py` |
| Add a kind of hero | a function under `@register(HEROES, …)` in `heroes.py` |
| Add a shape of generated prop | a function under `@register(SHAPES, …)` in `props.py` |
| Add a zone to a biome | `TEMPLATES` in `biomes/<name>/templates.py` |
| Change what LDtk receives | `generator/procedural/export_ldtk.py` |
