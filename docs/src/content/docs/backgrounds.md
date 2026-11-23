---
title: Writing a background
description: How to describe what the client draws behind the game in assets/backgrounds/, the keys of a layer and of its content, the rules between them, and the error the client prints when a file is wrong.
sidebar:
  label: Backgrounds
---

A background, `assets/backgrounds/<name>.toml`, describes what the client draws behind the game: the sky of the menus, the walls of a biome. This page is for whoever adds or tunes one: it gives the keys, what each one does on screen, the rules between keys, and how to read the error the client prints, without opening the C++. How the client draws a background is told in [Client](/R-Type/client/#sky-sky).

The client reads the folder `backgrounds` (`BACKGROUND_FOLDER_IN_ASSETS`) of its `assets/` folder once, when it starts, and loads the images the files name. The name of a file without `.toml` is the name of the background. A level shows the background named after its biome (`level.biome(name)`, see [Scripting in Luau](/R-Type/scripting/)); without a biome, behind the menus, and for a biome no file is named after, the client shows `space` (`DEFAULT_BACKGROUND_NAME`).

| Part | State | Where |
|---|---|---|
| Backgrounds of the client: `space`, the sky, `fosse`, the Pit, the four biomes of the Ashlands, `cendres`, `cendres_abri`, `cendres_coeur` and `cendres_silo`, and the five of Deseret, `deseret`, `deseret_hoodoos`, `deseret_line`, `deseret_mine` and `deseret_mesa` | Available | `assets/backgrounds/` |
| A background a resource of a server brings, sent to the players | Available | `resources/<name>/backgrounds/`, see [A background of a resource](#a-background-of-a-resource) |

## A file

A background is a list of layers, from the farthest to the nearest. Each layer moves at its own speed and shows one content: an image, the tiles of an LDtk layer, particles, or props placed in LDtk. Places and sizes are in background pixels, the background being 480 by 270 (`BACKGROUND_SIZE`); speeds are in background pixels per second.

```toml
[[layer]]
name = "planet"
motion = { autoscroll = 1.8, repeat = 647 }
image = { file = "backgrounds/space/planet.png", x = 288, y = 43 }

[[layer]]
name = "far stars"
motion = { autoscroll = 4.5, repeat = 488, warps = true }

[layer.particles]
population = 441
area = { left = 0, top = 0, width = 480, height = 270 }
pixel = "deep_blue"
twinkle = "blue"
twinkle_steps = 8
```

A table in braces fits on one line; one that needs several lines is written as a table of its own, `[layer.particles]`, after the keys of its layer.

A background may also name, above its layers, the music a game plays while it shows, by its asset id: `music = "music/crawl_out_through_the_fallout.mp3"` in the four backgrounds of the Ashlands, `music = "music/big_iron.mp3"` in the five of Deseret, `music = "music/mistaken_identity.mp3"` in the Pit, a file of `assets/music/`. It plays in place of the music of a game from the first status of the level that shows the biome until the game ends. The alarm of a boss stops it; it comes back once the boss is defeated, or at the end of the alarm when the sheet of the boss names no music of its own (`boss_music`, see [Client](/R-Type/client/#sounds-and-music-iaudioservice-soundplayer-musicplayer)). A background without `music` leaves the music of a game. A resource of a server does not send the file of a music: the client looks for it in its own assets.

## A layer

| Key | Needed | What it is |
|---|---|---|
| `name` | yes | how the journal names the layer |
| `motion` | no | how the layer moves; without it, it stays where it is |
| `entrance` | no | `true` on the layer the background arrives through; one layer at most |
| `image`, `tiles`, `particles` or `props` | exactly one | what the layer shows |

`motion` holds, each of which it may leave out:

| Key | Range | What it does |
|---|---|---|
| `parallax` | 0 to 4 | the share of the scroll of the level the layer follows: 0 stays still, 1 moves with the playfield, 0.25 lags behind as a far wall does |
| `autoscroll` | -4096 to 4096 | pixels per second the layer moves to the left on its own, whether the level scrolls or not; a negative one moves to the right |
| `repeat` | 1 to 65536 | the width after which the layer shows again what it showed |
| `warps` | `true` or `false` | whether its autoscroll speeds up while the ships warp at the start of a game |

A layer with a `parallax` of 1 or more is painted on whole pixels, on the grid of the ships; any other moves by a pixel of the screen at a time, so a slow layer never jumps.

A background with an `entrance` arrives from the right edge of the screen through that layer. The layers written before it show only right of where the entrance begins, fading in over 16 columns, and the background it replaces stays behind until the entrance has left the screen. A background without one replaces the other at once.

## An image

`image` shows one image: `file`, its asset id, and `x` and `y`, where its top-left corner lies, 0 without them. In a layer that repeats, a copy that left on the left is followed by one that comes from the right.

## Tiles and props

`tiles` shows one tile layer of an LDtk project, its maps laid end to end, the first coming back after the last. `props` shows one entity layer the same way: each entity that has a tile is drawn as the rectangle its tile names, its pivot where the entity lies. Both hold three keys, all needed:

| Key | What it is |
|---|---|
| `project` | the asset id of the LDtk project |
| `layer` | the name of the layer in the project |
| `tileset` | the asset id of the image the tiles are cut from |

A project that cannot be read, or that lacks the layer, is told once in the journal of the client, and its layer shows nothing. How the maps of a biome are made is told in [Generating the maps of a biome](/R-Type/procedural-biome-maps/).

## Particles

A layer of particles is either a population or the particles of a group, never both.

- A **population** holds as many particles as `population` says, always there. They only move with their layer, and each one that leaves on the left comes back from the right at a new height: stars.
- The particles of a **group** are born one at a time, fly on their own and end: a shooting star, an asteroid.

Both hold where a particle is born, `area`, a table of `left`, `top`, `width` and `height`, and how it is drawn: `pixel`, a color of the palette, or `sprites`.

| Key of a population | Range | What it does |
|---|---|---|
| `population` | 1 to 4096 | how many particles |
| `twinkle_steps` | 1 to 1024 | a particle twinkles during one step of a third of a second out of this many |
| `twinkle` | a color | the color a `pixel` twinkles in; needed with `pixel` and `twinkle_steps`, refused otherwise. A sprite shows its second frame instead |
| `warp_trail` | a color | the color of the streak behind a particle while its layer warps |

| Key of a group | Range | What it does |
|---|---|---|
| `group` | a name | the `[[group]]` that gives birth to the particles |
| `weight` | 0.001 to 1000 | the chance of this layer among the layers of its group; 1 without it |
| `speed` | `[lowest, highest]`, 0 to 8192 | pixels per second, drawn for each particle; it stays without it |
| `angle` | `[lowest, highest]`, 0 to 360 | degrees: 0 flies to the right, 90 down, 180 to the left; 180 without it |
| `drift` | 0 to 8192 | the most a particle moves up or down per second, on top of its speed |
| `life_ms` | `[shortest, longest]`, 1 to 600000 | how long a particle lives, in milliseconds |
| `margin` | 0 to 65536 | a particle without `life_ms` ends once it is more than this many pixels out of the background |
| `trail` | a table | the trail it leaves along its path |

The particles of a group must end: they give `life_ms`, or a `speed` whose lowest is above 0.

A sprite is one image that holds frames of the same size side by side:

| Key | Needed | What it is |
|---|---|---|
| `file` | yes | the asset id of the image |
| `frames` | no | how many frames it holds; 1 without it |
| `weight` | no | its chance among the sprites of its layer; 1 without it |
| `anchor_x`, `anchor_y` | no | the pixel of a frame that lies on the particle; 0 without them |
| `fps` | no | `[lowest, highest]`: the frames loop at a rate drawn in that range |

Without `fps`, a particle that has a life shows its frames once over that life, and any other shows the first.

`trail` holds four keys, all needed: `longest`, in pixels, `grow` and `shrink`, in pixels per second, and `stops`, its colors from the particle to its end, each a table of `until`, a share of the trail from 0 to 1, and `color`. The trail grows while the particle lives, then shrinks, and the particle is gone once it is.

## Emission groups

A `[[group]]` gives the layers that name it one pace and one ceiling, so the sky never shows more of them than it should. All four keys are needed:

| Key | Range | What it is |
|---|---|---|
| `name` | a text | the name the layers write in `group` |
| `most_alive` | 1 to 64 | the most particles of its layers alive at once |
| `first_ms` | 0 to 600000 | milliseconds before the first birth |
| `every_ms` | `[shortest, longest]`, 1 to 600000 | milliseconds drawn between a birth and the next |

At each deadline, the group draws one of its layers by their weights and a particle is born there. While `most_alive` particles live, the deadline waits, and the birth happens as soon as one of them ends.

## Colors and files

A color is one of five names (`PALETTE_NAMES`): `deep_blue`, `blue`, `pale_blue`, `star_white` and `star_gold`. Any other look is an image.

A file is named by its asset id: lowercase letters, digits and `_-.`, its folders separated by `/`, from the `assets/` folder (see [Client](/R-Type/client/#assets-the-folder-and-the-ids)). A file the folder does not hold stops the client when it starts. In a resource, the same id is the path of the file from the folder of the resource.

## A background of a resource

A resource of a server may bring backgrounds: `resources/<name>/backgrounds/<background>.toml`, written as any other, with the images and the LDtk projects it names in the folder of the resource. The supervisor adds them to the content pack its players download (see [Protocol](/R-Type/protocol/#content-pack)), so a player sees a new background without a new client.

| What the resource holds | What happens |
|---|---|
| A background whose name no background of the client has | it is added: `level.biome("<name>")` shows it |
| A background named as one of the client, `space` or `fosse` | it replaces it for the players of that server; of two resources that hold the same name, the last started wins |
| A file a background names | it is sent when a resource holds it, read in the last started that does; a file only the assets hold is not sent, every client has it |

- The backgrounds of the assets are never sent, and a resource replaces none of their files alone: a server changes a background by bringing a whole one of the same name.
- A background that names a file no folder holds stops the supervisor at its start, with the file.
- A path of the pack holds at most 64 characters, and the pack 510 files and 8 MiB, sheets and sprite sheets included.

The client reads the pack when the server accepts it, and a background it cannot read refuses the pack: the player does not join. The loading screen of a game loads the images, and the sky shows the backgrounds of the pack from then to the end of the session; before the first game, the waiting room still shows the sky of the client. How the client lays them over its own is told in [Client](/R-Type/client/#from-a-file-to-the-screen).

## When a file is wrong

The client reads a background as strictly as an entity sheet: a key a table does not know, a missing key, a value of another kind or out of its range each stop it when it starts, with the file, the line, the column and the key:

```text
/…/assets/backgrounds/space.toml:12:1: layer[1].particles: particles hold either population or group
```

A file holds at most 32 layers and 8 groups, a layer of particles 16 sprites and a trail 16 stops.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change the sky, the Pit or a biome of the Ashlands or of Deseret | `assets/backgrounds/space.toml`, `assets/backgrounds/fosse.toml`, `assets/backgrounds/cendres*.toml`, `assets/backgrounds/deseret*.toml` |
| Add a background for a biome | a file `assets/backgrounds/<biome>.toml`, and `level.biome("<biome>")` in the script of the level |
| Bring a background with a resource | `resources/<name>/backgrounds/`, see [A background of a resource](#a-background-of-a-resource) |
| Change which files of a background a pack carries | `backgroundFilesIn()` in `src/game/Backgrounds/BackgroundFiles.cpp` |
| Add a key, or change a range | `BackgroundFileConstants.hpp` and `BackgroundReading.cpp` in `src/client/Rendering/Background/Description/` |
| Add a color to the palette | `PALETTE_NAMES` and `PALETTE_COLORS` in `BackgroundFileConstants.hpp` |
| Change how a content is drawn | its class in `src/client/Rendering/Background/Layers/` |
