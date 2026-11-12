# Biome maps

Generates background maps for a biome, one 480 × 272 px screen per zone, from zone templates written by hand, and exports them as an LDtk project. A map that breaks a level design rule is never written.

A biome is a folder of data in `biomes/`. Two are provided: `fosse`, the Pit, and `demo_ember`, a demonstration of how a biome brings its own materials and assets.

## Commands

```bash
python3 -m pip install -r requirements.txt
```

```bash
python3 generate_maps.py --biome fosse --count 6 --zones 3 --seed 57 --out out
```

```bash
python3 export_ldtk.py --biome fosse --maps out --out ldtk
```

```bash
python3 tools/verify_ldtk.py --biome fosse --maps out --project ldtk --schema JSON_SCHEMA.json
```

Every script runs from this folder or from anywhere else; `--out`, `--maps` and `--project` are relative to the current directory.

`generate_maps.py` options:

| Option | Meaning |
|---|---|
| `--biome` | name of a folder of `biomes/` (default `fosse`) |
| `--count` | number of maps; map number i uses the seed `seed + i` |
| `--zones` | screens per map (width = 480 × zones) |
| `--seed` | first seed; the same seed always gives the same map |
| `--out` | output folder: `map_<seed>.png`, `map_<seed>.json` (the seed really used, the zones, the forced chain if any) and `contact.png` |
| `--layers` | also writes the three lit planes of each map as transparent pictures; stacked, they give the map |
| `--sequence a,b*,c` | forces the chain of templates; a trailing `*` mirrors one |

When a map fails a check, the script tries a derived seed (`seed + 7919 × attempt`, 20 attempts at most) and prints why each attempt was rejected. For each map it writes, it prints the measure of every rule.

`tools/preview_biome.py --biome <name> --out preview.png` draws a swatch of each material of a biome and every prop it generates by rule.

## Rules

| No. | Rule | Source | Checked |
|---|---|---|---|
| 1 | A map is a chain of zones drawn from hand-written templates with typed connectors | art direction | by construction |
| 2 | Terrain first, decor afterwards | The Level Design Book | by construction |
| 3 | Nothing floats | art direction | yes: flood fill from the borders on each plane, and support under every prop |
| 4 | Columns stand on a material that bears them, out of the water, in rows of three or more | art direction | yes |
| 5 | Water only in closed basins; a bank goes on under the water in steps | art direction | yes |
| 6 | A waterfall has a source and a pool; one at most every three zones | art direction | yes |
| 7 | One main material per zone (60 % at least); materials change at seams | art direction, The Level Design Book | yes |
| 8 | The play corridor stays clear (see "Frame") | art direction | yes: five counts, all zero |
| 9 | Decor in clusters (a large piece, a medium one touching it, a small one apart), empty space between clusters, no even spacing | The Level Design Book, Pixel Sanctuary | yes |
| 10 | Never the same sprite twice within 160 px on a plane, nor within 96 px across the two terrain planes; grass and strands drawn stroke by stroke | art direction, Pixel Sanctuary | yes |
| 11 | One hero per zone, on a third of the screen (within 24 px) | The Level Design Book | yes |
| 12 | Seams hide behind a rock or a pier | The Level Design Book | yes, for dry changes of material |
| 13 | Quiet colors; vivid tones kept for what glows | The Level Design Book | partly: vivid pixels under 6 % |
| 14 | Three planes, decor on each | art direction | yes |
| 15 | Light as a separate pass, a source in every zone, moods that differ | art direction | yes for the sources and the moods; the quantized, dithered bands are by construction |
| 16 | No geometric shape with a one-pixel outline, no smooth surface, no curve drawn by a formula, everything at 1x | art direction | no: judged by eye |
| 17 | Nothing that matters under the status band of the HUD (y ≥ 246) | `PLAY_AREA_HEIGHT` of the game | yes: heroes, light sources, foam |

Sources: The Level Design Book, "Environment Art" (https://book.leveldesignbook.com/process/env-art); Pixel Sanctuary, "Environmental storytelling and set dressing" (https://www.pixelsanctuary.com/tutorials/environmental-storytelling-set-dressing).

What the checks let through: even spacing is tested between free clusters only, since a hero and a seam cover have a place set by rules 11 and 12; the columns of a colonnade repeat by nature and do not count as a repeated sprite.

## Frame

`frame.py` holds what every biome shares, in pixels.

| Measure | Value |
|---|---|
| Screen | 480 × 272 |
| Nothing solid on the play plane | from y = 48 to y = 212 |
| Lower edge of the ceiling of the play plane | y = 40 at the lowest |
| What hangs from that ceiling | down to y = 72; a longer sprite is cropped from its top |
| Props of the play plane | none between y = 96 and y = 176 |
| Ground of the play plane | y = 217 at the highest |
| Second plane | ceiling down to y = 64, ground up to y = 196 |
| Status band of the HUD | y ≥ 246 |

The columns of a colonnade, the piers of the second plane and the shelf a waterfall passes behind go past the limits of the second plane on purpose.

## Structure of a biome

| File of `biomes/<name>/` | Content |
|---|---|
| `__init__.py` | empty |
| `palette.py` | `NEAR` and `MID` (one letter per color, the same letters on both planes), `BACKDROP`, `FAR_A`, `FAR_B`, `FAR_LIT_KEYS`, the tints of the lights, and `OUTLINE` (how the import tool softens a dark outline) |
| `materials.py` | `MATERIALS`: for each material its painter, the letters and settings of that painter (`paint`), whether it lies on an 8 px grid (`aligned`), its moss, grass and hanging growth, what covers its seams, and `"props": True` (or a list of shapes) when it builds props by rule; `SEAM_MATERIAL` |
| `elements.py` | `WATER`, `FALL`, `LAMPS`, `COLUMN`, `STROKES`, `SPECKS`, `WET_CONNECTORS`, `WALL_KEY` |
| `decor.py` | only what file names cannot say: `SPRITES_FROM` to borrow the sprites of another biome, and `FAMILIES`, `EMITTERS`, `FAR_SETS`, `COLUMN_SPRITES` for sprites whose file names carry no role; every name is optional |
| `templates.py` | `TEMPLATES`: the zones, their connectors, their hero, the places of their clusters |
| `sprites/`, `sprites.json` | the sprites, in the exact colors of `NEAR`, and their index |
| `sheets.py`, `raw/` | sheets to cut and how to cut them (optional) |
| `drawn.py` | sprites drawn in palette letters (optional) |

Registered names a biome can refer to:

| Kind | Names |
|---|---|
| Painters | `cobble` (packed stones from a texture), `soil` (dark earth with a dithered edge), `bricks` (courses of blocks), `plates` (large riveted plates) |
| Heroes | `colonnade`, `cluster`, `waterfall`, `light_well` |
| Moss covers | `full`, `patches` |
| Hanging growth | `strands`, `teeth`, `drips` |
| Far structures | `colonnade`, `pier` |
| Prop shapes | `pile`, `wall`, `steps`, `slab`, `pillar`, `pillar_broken`, `drum`, `lintel` |

Settings of `bricks`: `cell` (size of a cell), `span` (cells per block), `bond` (`running` or `stack`), `bevel` (2 or 1), `chips`, `tall_chance`, `tints` (shares of darker and lighter blocks), `stone` (the six letters of the block) and `frame` (the bands of courses on a full screen). Settings of `plates`: `plate` (size), `stagger`, `keys`.

A template with `"sky": True` has no ceiling: the ceiling of a neighboring zone ends in three steps, nothing hangs, piers become spires. The heroes `waterfall` and `light_well` need a ceiling; a `colonnade` under an open sky must give its `height`.

## Adding a biome

1. Copy `biomes/demo_ember/` and rename it. Write the palette first: every other file refers to its letters. `python3 tools/make_palette.py --biome <name> path/to/pictures` builds it from pictures: it finds their color ramps and fills the standard letters, so the files copied from `demo_ember` keep working; `--assign stone=1,liquid=0` overrules the role a ramp got.
2. Bring sprites in. Pictures with a transparent background, one sprite per file:

```bash
python3 tools/import_sprites.py --biome <name> path/to/pictures
```

A sheet of sprites separated by empty gutters:

```bash
python3 tools/import_sprites.py --biome <name> sheet.png --grid 4x4 --names a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p
```

The tool crops each sprite, moves each pixel to the nearest color of the palette (`--keys` limits the letters allowed), replaces the dark outline on the lit side, and updates `sprites/` and `sprites.json`. `--sheets` cuts the sheets described in `sheets.py`; `--drawn` renders the sprites of `drawn.py` and imports them.

3. Name a sprite `<family>.<size>.<name>[.glow][.hang][.hero].png` and it needs no line of `decor.py`: `size` is `large`, `medium` or `small`; `glow` makes it a light whose tint comes from its brightest pixels and whose reach comes from its size; `hang` keeps it under ceilings; `hero` keeps it for the hero of a zone. A cluster of family `forge` then draws from `forge.large.*`, `forge.medium.*` and `forge.small.*`. What `decor.py` declares wins over what the names say.
4. Props generated by rule need no sprite. A material with `"props": True` builds `gen:<material>:<shape>` for each shape, painted by its own painter with its own settings, and gets a family named after it. A generated prop is used like any sprite, in a family, a seam cover or a hero. Changing the material changes the props.
5. Write the templates: a material, two connectors (neighboring zones must carry the same name; names listed in `WET_CONNECTORS` let the water through), the four profiles as `(x, y)` steps, a hero on a third (x = 160 or 320), the places of the clusters, an ambient level and a mood.
6. Generate, look, export:

```bash
python3 generate_maps.py --biome <name> --count 3 --zones 3 --seed 1 --out out_<name>
```

```bash
python3 export_ldtk.py --biome <name> --maps out_<name> --out ldtk_<name>
```

Code is needed only when the registries fall short. A material unlike the four painters: write `paint_<name>(scene, canvas, mask, shared, options)` in `painters.py` under `@register(PAINTERS, "<name>")`. A subject of a new kind: write `<name>(cave, zone, x, hero)` in `heroes.py` under `@register(HEROES, "<name>")`. A new prop shape: a function that returns a mask, in `props.py`, under `@register(SHAPES, "<name>")`.

## The two biomes

`fosse`: three materials (rock, earth, masonry), eight templates, water, 48 sprites and a rock texture. Its sprites were generated with SpriteCook, then cut and snapped to the palette by `tools/import_sprites.py --sheets`; the sheets as received are in `raw/` and their identifiers in `spritecook-assets.json`.

`demo_ember`: a demonstration of the mechanism, not a drawn biome. It has its own palette, a material of riveted iron (`plates`), a firebrick that is the `bricks` painter with other settings, props generated from both, seven small sprites drawn in letters in `drawn.py` and imported by file name, lava in place of water, and a zone under an open sky. It borrows the rest of its sprites from `fosse`.

## LDtk export

The project follows the JSON format of LDtk 1.5.3.

| Parameter | Value |
|---|---|
| Grid | 16 px everywhere |
| Levels | one per map, `<Biome>_<seed>`, 480 × zones by 272 px, saved inside the project file |
| World | free layout, levels side by side, 32 px apart |
| Entity pivot | (0, 0) |
| Layers, top to bottom | `Mobs` (entities, empty), `Walls` (IntGrid, 1 = `wall`, hidden when the project opens, shown at 35 %), `Near`, `Mid`, `Far` (tiles) |
| Tilesets | `<biome>_near.png`, `<biome>_mid.png`, `<biome>_far.png` (one atlas per plane for every map), and `props_16.png`, which no layer uses; 16 px tiles, no padding, no spacing, paths relative to the project |
| Identifiers | derived from names: two exports give the same file |

A `Walls` cell is 1 when at least half of its pixels are solid on the play plane; water is not a wall. The light is baked in the tiles, so almost no tile repeats: the three atlases give the picture back, and `props_16.png` is there to paint by hand. `Far` also holds the waterfall; `Near` also holds the water, the foam and the floating specks.

`tools/verify_ldtk.py` validates the project against the official schema (download https://ldtk.io/files/JSON_SCHEMA.json and pass it with `--schema`; it needs the `jsonschema` package), rebuilds each level from its tiles and compares it with the map pixel by pixel, and checks that no `Walls` cell lies in the play corridor. `--sample <project.ldtk>` also compares field names with a project saved by LDtk.

## Not supported

- A biome without ground: the terrain, the placing of decor and the rules assume ground under every column.
- Looping parallax layers: this tool makes fixed maps.
- A drawn sky: under `"sky": True` the backdrop is one flat color.
- More than two planes of terrain, or platforms across the play corridor.
- Two biomes in one output folder: `props_16.png` has the same name for all.
