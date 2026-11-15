# Biome maps

Generates background maps for a biome, one 480 × 272 px screen per zone, from zone templates written by hand, and exports them as an LDtk project. A map that breaks a level design rule is never written.

This folder holds the tool only: the engine, its registries, the LDtk export and a few tools. It ships without any biome. A biome is a folder of data, one `biome.json` and its sprites, kept outside the repository on the machine of whoever draws it; maps are generated locally. Every command takes `--biomes <folder>`, the folder that holds one folder per biome, and `--biome <name>`; both are required.

## Commands

```bash
python3 -m pip install -r requirements.txt
```

```bash
python3 tools/new_biome.py --biomes path/to/my_biomes <name>
```

```bash
python3 generate_maps.py --biomes path/to/my_biomes --biome <name> --count 6 --zones 3 --seed 57 --out out
```

```bash
python3 export_ldtk.py --biomes path/to/my_biomes --biome <name> --maps out --out ldtk
```

```bash
python3 tools/verify_ldtk.py --biomes path/to/my_biomes --biome <name> --maps out --project ldtk --schema JSON_SCHEMA.json
```

Every script runs from this folder or from anywhere else; `--biomes`, `--out`, `--maps` and `--project` are relative to the current directory. A folder of biomes or a biome that does not exist, a `biome.json` that is not valid JSON, and a key every biome must have and this one lacks, each end the command with one line that names the file and what is wrong.

`generate_maps.py` options:

| Option | Meaning |
|---|---|
| `--biomes` | folder that holds one folder per biome (required) |
| `--biome` | name of a biome, that is a folder of that folder (required) |
| `--count` | number of maps; map number i uses the seed `seed + i` |
| `--zones` | screens per map (width = 480 × zones) |
| `--seed` | first seed; the same seed always gives the same map |
| `--out` | output folder: `map_<seed>.png`, `map_<seed>.json` (the seed really used, the zones, the forced chain if any) and `contact.png` |
| `--layers` | also writes the three lit planes of each map as transparent pictures; stacked, they give the map |
| `--sequence a,b*,c` | forces the chain of templates; a trailing `*` mirrors one |

When a map fails a check, the script tries a derived seed (`seed + 7919 × attempt`, 20 attempts at most) and prints why each attempt was rejected. For each map it writes, it prints the measure of every rule.

`tools/preview_biome.py --biomes <folder> --biome <name> --out preview.png` draws a swatch of each material of a biome and every prop it generates by rule.

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

## Layouts

A template names its layout with `"layout"`; without it the layout is `corridor`. A layout builds the zones, the terrain and the decor of a map, and has its own list of checks.

| Layout | Terrain | Checks |
|---|---|---|
| `corridor` (default) | a floor and a ceiling on two planes, read from the step profiles of the template | the rules above |
| `field` | no floor and no ceiling: floating bodies painted with the materials of the biome | the rules below |

The two layouts do not mix in one map: joining them would mean rewriting the ground of the corridor zone next to the seam. A chain that mixes them is refused. Templates of the `field` layout carry the connector `open` on both sides, so a drawn chain never mixes them with the templates of a corridor.

### The `field` layout

A body is a silhouette cut in steps: a top edge and a bottom edge that each fall from a plateau set off-center, with a notch or two. No curve comes from a formula and no two bodies are alike. Bodies are painted by the painter of their material, with depth counted from the top left of each body, so the lit texture sits on the top left and the dark core on the far side. Marks a biome draws for a material (`elements.marks`, craters for instance) are stamped on the lit face.

| Plane | What it holds |
|---|---|
| Far | banks of haze in two flat tones with a dithered rim, stars in three intensities crowded along a wandering band, and small two-tone silhouettes |
| Second | the hero body of the zone, then groups of bodies anywhere between y = 12 and y = 238 |
| Play | only bodies that show past the top of the screen (down to y = 40 at most) or past its bottom (from y = 217); the corridor stays empty |

Bodies come in groups: a large one, a medium one 4 to 9 px from it, a small one 16 to 32 px away, then at least 22 px of empty space. Decor stands on the top of a body or hangs under it, placed by the same clusters as in a corridor.

Heroes of this layout: `great_body`, the largest body of the zone with a glowing vein and a cluster on its top, and `wreck`, a hull built on an 8 px grid with the material the template names, a row of portholes and a beacon. Both give the zone a light.

Checks of the `field` layout:

| No. | Check |
|---|---|
| 3 | Each body is in one piece. Two bodies of a plane neither overlap nor touch. A body of the play plane reaches the top or the bottom of the screen and is never cut by a zone edge: it stops before the edge or goes on in the next zone. Every prop has a body under it |
| 4, 5, 6, 12 | Columns, water, waterfalls and seams: printed as `n/a`, not as passed |
| 7 | One main material per zone (60 % at least) |
| 8 | Nothing solid on the play plane between y = 48 and y = 212, no upper body below y = 40, no lower body above y = 217 |
| 9 | Decor in clusters, as in a corridor; bodies in groups as described above, with no even spacing between groups that lie on one row (within 48 px in height) |
| 10 | No sprite twice within 160 px; no body silhouette twice within 160 px on a plane (silhouettes under 48 pixels are too small to tell apart and are not compared) |
| 11, 13, 15, 17 | One hero per zone on a third, quiet colors, a light in every zone, nothing that matters under y = 246: unchanged |
| 14 | Bodies on the far, second and play planes of every zone |

A `field` template:

| Key | Meaning |
|---|---|
| `layout`, `material`, `left`, `right`, `ambient`, `mood` | `"field"`, the material of the bodies, `"open"` twice, the light |
| `hero` | `kind` (`great_body` or `wreck`), `x` on a third, `y`, `size`, `silhouette` (`boulder` or `hull`), `material`, `family` and `lead` of its cluster, then `vein`, or `portholes` and `beacon` |
| `mid_bodies` | `[x, y, family]` per group of the second plane; the family dresses the large body, `null` leaves it bare |
| `near_top`, `near_bottom` | `[x, family]` per group of the play plane |
| `far_bodies` | `[x, y]` per group of far silhouettes |

A material used only by this layout needs `painter`, `paint` and `aligned`; moss, grass, hanging growth and seam covers belong to the corridor. `elements` may hold `stars`, `haze` and `marks`; without them the backdrop is flat and the bodies bare.

A third layout is a function `build_<name>(cave, sequence)` under `@register(LAYOUTS, "<name>")`, which fills the zones, the solid masks, the canvases and the decor of the map, and a list of checks under its name in `CHECKS` of `rules.py`.

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

## The data file of a biome

A biome is the folder `<biomes>/<name>/`:

| Entry | Content |
|---|---|
| `biome.json` | everything the biome declares, in the keys below |
| `sprites/`, `sprites.json` | its sprites, in the exact colors of `palette.near`, and their index; both may be empty |
| `raw/`, `sources.json` | sheets to cut, and where they came from (optional) |

A list in the file reaches the engine as it is written; a string that starts with `@` stands for the tint of that name in `palette.tints`, so `"mood": "@WARM"` and a lamp with `"tint": "@WARM"` share one value.

| Key of `biome.json` | Content |
|---|---|
| `palette.near` | one letter per color on the play plane; every other key refers to colors by their letter |
| `palette.mid` or `palette.mid_share` | the same letters on the second plane, written out, or the share of the way from the backdrop to `near` (0.5 when neither is given) |
| `palette.backdrop`, `palette.far_a`, `palette.far_b`, `palette.far_lit_keys` | the color behind everything, the two tones of the far plane, and the letters of a sprite that take the lighter tone when it is drawn as a far silhouette |
| `palette.outline` | how the import tool softens a dark outline: `dark_through` (the darkest letters) and `rim` (the letter that replaces them on the lit side); optional |
| `palette.tints` | named tints of light, three numbers from 0 to 1 |
| `materials.<name>` | `painter` and `paint` (its letters and settings), `aligned` (true when it lies on an 8 px grid), `props` (true, or a list of shapes, when it builds props by rule); for a corridor also `moss`, `grass`, `hanging`, `seam_floor`, `seam_ceil`, and optionally `broken_edges`, `cracks`, `bears_columns` |
| `seam_material` | the material of the pier that hides a seam |
| `elements` | `strokes`, `specks` and `wall_key` (required); `water`, `fall`, `lamps`, `column`, `wet_connectors` for a corridor; `stars`, `haze`, `marks` for a field |
| `decor` | only what file names cannot say: `sprites_from` (borrow the sprites of another biome of the same folder), `families`, `emitters`, `far_sets`, `column_sprites`; every key is optional |
| `templates.<name>` | a zone: `material`, `left` and `right` connectors, `ambient`, `mood`, `hero`, then the keys of its layout |
| `drawn` | sprites drawn as rows of palette letters, `.` for an empty pixel, named like sprite files (optional) |
| `sheets` | `sheets`, `textures` and `references`: how `import_sprites.py --sheets` cuts the pictures of `raw/` (optional) |

A short extract, the palette and one template of the biome `new_biome.py` writes:

```json
{
  "palette": {
    "near": {"0": [14, 14, 22], "1": [26, 28, 44], "2": [40, 44, 66], "3": [58, 64, 92], "t": [70, 96, 110], "o": [255, 196, 110]},
    "mid_share": 0.5,
    "backdrop": [10, 10, 18],
    "far_a": [16, 17, 28],
    "far_b": [26, 29, 46],
    "far_lit_keys": "45tc",
    "tints": {"WARM": [1.0, 0.75, 0.45], "COOL": [0.6, 0.8, 1.0]}
  },
  "templates": {
    "hall": {
      "material": "stone", "left": "low", "right": "low", "ambient": 0.8, "mood": "@WARM",
      "hero": {"kind": "cluster", "x": 160, "family": "stone", "plane": "near", "surface": "floor", "lead": "gen:stone:pillar"},
      "floor": [[0, 232], [120, 224], [280, 232]],
      "ceil": [[0, 32], [160, 40], [320, 32]],
      "mid_floor": [[0, 208], [200, 200], [360, 208]],
      "mid_ceil": [[0, 56], [240, 48]],
      "near_floor": [[380, "stone"]],
      "mid_floor_decor": [[90, "stone"], [300, "stone"]]
    }
  }
}
```

Registered names a biome can refer to:

| Kind | Names |
|---|---|
| Painters | `cobble` (packed stones from a texture), `soil` (dark earth with a dithered edge), `bricks` (courses of blocks), `plates` (large riveted plates) |
| Layouts | `corridor`, `field` |
| Heroes | `colonnade`, `cluster`, `waterfall`, `light_well`; in a `field`: `great_body`, `wreck` |
| Body silhouettes | `boulder`, `hull` |
| Moss covers | `full`, `patches` |
| Hanging growth | `strands`, `teeth`, `drips` |
| Far structures | `colonnade`, `pier` |
| Prop shapes | `pile`, `wall`, `steps`, `slab`, `pillar`, `pillar_broken`, `drum`, `lintel` |

Settings of `bricks`: `cell` (size of a cell), `span` (cells per block), `bond` (`running` or `stack`), `bevel` (2 or 1), `chips`, `tall_chance`, `tints` (shares of darker and lighter blocks), `stone` (the six letters of the block) and `frame` (the bands of courses on a full screen). Settings of `plates`: `plate` (size), `stagger`, `keys`.

A template with `"sky": true` has no ceiling: the ceiling of a neighboring zone ends in three steps, nothing hangs, piers become spires. The heroes `waterfall` and `light_well` need a ceiling; a `colonnade` under an open sky must give its `height`.

A template with `"entrance": {"mouth": 120}` opens a map as the entrance of a cave: left of its mouth, at x = 120 here, the play plane is cut along a ragged rock edge and draws nothing, so the game shows the open space the team flies in from, and its walls end there too. The far and second planes keep their pixels: the client shows them only right of the mouth. An entrance is never drawn into a chain: it only opens a map whose chain `--sequence` forces, as its first template, never mirrored; anywhere else the command stops. The project of the Pit opens its first map with `cave_mouth`.

## Creating a biome

1. Write a starting biome. It runs at once: a palette, one material that builds its own props by rule, two templates, no sprite. `--layout field` starts a biome of open space instead of a corridor.

```bash
python3 tools/new_biome.py --biomes path/to/my_biomes <name> --layout corridor
```

2. Give it its colors. Every other key refers to the letters of the palette, so the palette comes first. This reads the color ramps of pictures and fills the standard letters in `biome.json`, keeping its tints; `--assign stone=1,liquid=0` overrules the role a ramp got.

```bash
python3 tools/make_palette.py --biomes path/to/my_biomes --biome <name> path/to/pictures
```

3. Bring sprites in. Pictures with a transparent background, one sprite per file:

```bash
python3 tools/import_sprites.py --biomes path/to/my_biomes --biome <name> path/to/pictures
```

A sheet of sprites separated by empty gutters:

```bash
python3 tools/import_sprites.py --biomes path/to/my_biomes --biome <name> sheet.png --grid 4x4 --names a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p
```

The tool crops each sprite, moves each pixel to the nearest color of the palette (`--keys` limits the letters allowed), replaces the dark outline on the lit side, and updates `sprites/` and `sprites.json`. `--sheets` cuts the sheets `biome.json` describes under `sheets`; `--drawn` renders the sprites it draws in letters under `drawn` and imports them.

4. Name a sprite `<family>.<size>.<name>[.glow][.hang][.hero].png` and it needs no line of `decor`: `size` is `large`, `medium` or `small`; `glow` makes it a light whose tint comes from its brightest pixels and whose reach comes from its size; `hang` keeps it under ceilings and under bodies; `hero` keeps it for the hero of a zone. A cluster of family `forge` then draws from `forge.large.*`, `forge.medium.*` and `forge.small.*`. What `decor` declares wins over what the names say.
5. Props generated by rule need no sprite. A material with `"props": true` builds `gen:<material>:<shape>` for each shape, painted by its own painter with its own settings, and gets a family named after it. A generated prop is used like any sprite, in a family, a seam cover or a hero. Changing the material changes the props.
6. Write the templates. For open space, see "The `field` layout". For a corridor: a material, two connectors (neighboring zones must carry the same name; names listed in `elements.wet_connectors` let the water through), the four profiles as `[x, y]` steps, a hero on a third (x = 160 or 320), the places of the clusters, an ambient level and a mood.
7. Generate, look, export:

```bash
python3 generate_maps.py --biomes path/to/my_biomes --biome <name> --count 3 --zones 3 --seed 1 --out out_<name>
```

```bash
python3 export_ldtk.py --biomes path/to/my_biomes --biome <name> --maps out_<name> --out ldtk_<name>
```

Code is needed only when the registries fall short. A material unlike the four painters: write `paint_<name>(scene, canvas, mask, shared, options)` in `painters.py` under `@register(PAINTERS, "<name>")`. A subject of a new kind: write `<name>(cave, zone, x, hero)` in `heroes.py` under `@register(HEROES, "<name>")`. A new prop shape: a function that returns a mask, in `props.py`, under `@register(SHAPES, "<name>")`.

## What goes in the repository

The tool, this file and the page of the documentation site. A biome, its pictures and the maps generated from it stay local. When a biome is ready, the LDtk project exported from it and its atlases can be committed where the game reads them; nothing else needs to be.

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

- A biome without ground is supported by the `field` layout. Still not supported there: a map that mixes `field` and `corridor` zones, water, waterfalls, columns, hanging lamps, and bodies that cross the play corridor.
- Looping parallax layers: this tool makes fixed maps.
- A drawn sky: under `"sky": true` the backdrop is one flat color.
- More than two planes of terrain, or platforms across the play corridor.
- Two biomes in one output folder: `props_16.png` has the same name for all.
