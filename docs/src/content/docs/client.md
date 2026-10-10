---
title: Client
description: What the client does, how its parts fit together and where to find them in the code.
---

The client, `r-type_client` (`src/client/`), shows the game, plays its sounds and reads the player's input; the server decides what happens in the match (see [Architecture](/R-Type/architecture/)). Today it opens a window, finds its assets folder at launch, can load the assets of a screen and draws a scrolling starfield behind the playfield. The screens themselves do not exist yet.

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Window and frame loop | Available | `src/client/Window/` |
| Assets folder and asset ids | Available | `src/client/Assets/`, `assets/README.md` |
| Asset loading | Available; used at launch for the font of the window size buttons | `src/client/Assets/AssetLibrary/`, `src/client/Assets/AssetLoadingStep/` |
| Path of the running executable | Available | `src/client/Platform/` |
| Drawing entities | Available; not called by the frame loop yet | `src/client/Rendering/RenderSystem/`, `src/client/Rendering/SfmlDrawSurface/` |
| Scrolling background | Available; drawn by the frame loop | `src/client/Rendering/Background/` |
| Screens and the loading screen | Not yet written | — |
| Sprites, sounds and music of the game | Not yet written | — |

## Assets: the folder and the ids

Every file the client reads at run time sits in `assets/`, at the root of the repository. At launch, `locateAssetFolder()` (`rtype_client_assets`) looks for `assets/` next to the executable, then in the folder above it, where the Visual Studio generator writes `Release/` and `Debug/`. The folder the client was launched from plays no part. When neither folder exists, the client stops with exit code 1 and an `AssetFolderNotFoundException` naming both folders it searched.

Code names a file by its **asset id**: its path in `assets/`, with `/` between folders, for example `sprites/player_ship.png`. Every folder and file name of an id starts with a lowercase letter or a digit, then holds only `a-z`, digits, `_`, `-` and `.`; `isValidAssetId()` checks it. An id is therefore spelled the same on every system and never names a file outside `assets/`, and a later mod folder can replace a file by holding the same path. Ids are written once, as named constants.

## Assets: loading per screen

`AssetLibrary` (`rtype_client_asset_library`) owns every loaded asset. `load(kind, id)` reads a texture, a sound or a font the first time only, and only locates a music file, which is streamed while it plays. It never throws: it returns an `AssetLoadResult` (`Loaded`, `InvalidId`, `MissingFile`, `UnreadableFile`). Its lookups, `texture()`, `sound()`, `font()` and `musicFile()`, never read the disk; an id that is not loaded throws `AssetNotLoadedException`. A reference stays valid until its asset is released, so a sprite, which keeps a reference to its texture, must be gone before its texture is released.

`AssetLoadingStep` (`rtype_client_asset_loading_step`) loads the `AssetList` of the next screen. It is built once the previous screen is destroyed: it releases every loaded asset the list does not name, keeps the ones both screens use, then `loadNext()` loads one asset per call, so a loading screen can be drawn between two files. After the last asset, it throws one `AssetLoadingException` naming every invalid id, missing file and unreadable file.

```cpp
rtype::client::AssetLoadingStep loading(
    assets, rtype::client::AssetList{.textures = {PLAYER_SHIP_TEXTURE},
                                     .sounds = {SHOT_SOUND}});
while (!loading.isFinished()) {
  loading.loadNext();
  drawLoadingScreen(loading.processedCount(), loading.assetCount());
}
const sf::Texture& ship = assets.texture(PLAYER_SHIP_TEXTURE);
```

`PLAYER_SHIP_TEXTURE`, `SHOT_SOUND` and `drawLoadingScreen` stand for an id constant and a loading screen that do not exist yet.

### What a loading step does

The caller creates the step, then calls `loadNext()` once per frame until `isFinished()`, drawing the progress in between. Today the caller is `main`, which runs one step at launch, before the window opens, for the font of the window size buttons. Once screens exist, the screen stack will run one step at every screen change.

```mermaid
sequenceDiagram
  accTitle: Loading the assets of the next screen
  accDescr: The caller creates a loading step with the asset list of the next screen. The step asks the asset library to release what the list does not name. Then, once per frame, the caller asks the step for the next asset; the step asks the library to load it, the library reads the file with SFML unless it is already loaded, and returns how it went. After the last asset the step is finished, or throws one error naming every problem.
  participant Caller
  participant Step as AssetLoadingStep
  participant Library as AssetLibrary
  participant Sfml as SFML
  Caller->>Step: create(list)
  Step->>Library: release unlisted
  loop once per frame
    Caller->>Step: loadNext()
    Step->>Library: load(kind, id)
    Library->>Sfml: read file
    Sfml-->>Library: asset
    Library-->>Step: result
  end
  Step-->>Caller: done, or error
```

## Assets in the game: from an entity to the screen

The engine knows nothing about assets, and the game only names them: an entity that is drawn carries a `Sprite` component (`rtype_game`) holding an asset id and a layer, so the server can say what an entity looks like without SFML. Each frame, `RenderSystem` (`src/client/Rendering/`) walks the entities that have a `Position` and a `Sprite`, layer by layer, and asks an `IDrawSurface` to draw each asset id at its position. `SfmlDrawSurface` turns the id into a texture through `ITextureSource`; an id with no texture is skipped and logged once.

```mermaid
flowchart TB
  accTitle: From an entity to the screen
  accDescr: The Sprite component of an entity names an asset id. RenderSystem passes the id to SfmlDrawSurface, which asks an ITextureSource for the texture. The asset library is meant to be that source, but the link is not written yet.
  Entity["Sprite component"] --> Render[RenderSystem]
  Render --> Surface[SfmlDrawSurface]
  Surface --> Source[ITextureSource]
  Source -. "not written yet" .-> Library[AssetLibrary]
```

Two links do not exist yet: no `ITextureSource` reads from `AssetLibrary`, and the frame loop does not call `RenderSystem`. Once they do, showing a new image takes four steps:

1. Put the file in `assets/sprites/`, named after the asset id rule.
2. Write its id once, as a named constant.
3. List the id in the `AssetList` of every screen that draws it.
4. Give the entity a `Sprite` with that id and its layer.

## Scrolling background: `ScrollingBackground`

`ScrollingBackground` (`rtype_client_scrolling_background`) fills the playfield behind everything else: three layers of stars drifting left at different speeds, a ringed planet, and random sky events. `GameWindow` moves it every frame by the real time elapsed since the previous frame, read from `engine::SystemClock`, so its speed does not depend on the frame rate. A frame longer than `MAXIMUM_BACKGROUND_STEP` (50 ms) moves it by 50 ms only, so the sky does not jump after a stall. A star that leaves on the left comes back on the right at a new height, so the sky never shows a seam.

The sky is cosmetic and local to each client: it is not made of entities, nothing of it crosses the network, and each client draws a different sky (the seed comes from `std::random_device`).

```cpp
ScrollingBackground background(std::random_device{}());
background.advance(now - previousFrameTime);
background.draw(window);
```

`draw()` expects a target whose view shows the playfield in its logical units, as `GameWindow` sets it up.

### What the sky holds

| Layer | Stars | Speed, in background pixels per second |
|---|---|---|
| Far | 441 | 4.5 |
| Middle | 90 | 12 |
| Near | 36 | 30 |

The planet drifts at `PLANET_SPEED` (1.8 background pixels per second), drawn in front of the far stars and behind the others. Sky events are shooting stars, flares, asteroids and comets: one starts every 0.8 to 2.6 seconds (`SHORTEST_SKY_EVENT_GAP`, `LONGEST_SKY_EVENT_GAP`), at most `MAXIMUM_SKY_EVENTS` (4) at once, and they animate `SKY_EVENT_CLOCK_RATE` (3) times faster than real time. The layers are `STAR_LAYERS` in `BackgroundConstants.hpp`; the events are in `SkyEvents/`.

### How it is drawn

The sky is painted at `BACKGROUND_SIZE`, 480 × 270 background pixels; a background pixel covers `BACKGROUND_PIXEL_SIZE` (4) logical units. It is enlarged without smoothing by the largest whole factor that fits the playfield on screen (`integerScaleFactor`: 3 in a 1600 × 900 window), then smoothed over the remainder only, so its pixels stay square and equal at every window size.

A halo is added on top: the sky blurred by a Gaussian of `HALO_STANDARD_DEVIATION` (3 background pixels), added at 70 % opacity (`HALO_TINT`). On a machine without shaders, or whose driver does not compile the halo shader, the sky is drawn without halo and the reason is logged once by the `Background` module. A render texture that cannot be created raises `RenderTextureNotCreatedException`.

On a Retina Mac, SFML 3 renders every window at its size in points, not in screen pixels, and macOS scales the image up, so the whole client looks softer there than on other screens.

### Tested without a screen

`Starfield`, which moves and paints the sky, and the sky events paint through `IPixelSurface` and never touch the graphics card, so `client_background_tests` runs them on the CI. `PixelBatch` turns the painted pixels into one `sf::VertexArray` of points. Only `GaussianBlur` and `ScrollingBackground` need OpenGL; they are checked on screen.

## Platform: the executable path

`executablePath()` (`src/client/Platform/`) returns the path of the running executable, or nothing when the system does not tell. Each system has its own source file, `ExecutablePathLinux.cpp`, `ExecutablePathMacOs.cpp` and `ExecutablePathWindows.cpp`, and CMake builds only the one that matches.

## Where to intervene

| You want to… | Look at |
|---|---|
| Add an asset | A file in `assets/sprites/`, `sounds/`, `music/` or `fonts/`, and its id in the asset list of the screen that uses it |
| Change the asset id rule | `isValidAssetId()` in `src/client/Assets/AssetIds.cpp`, `ASSET_ID_PUNCTUATION` in `AssetConstants.hpp`, and `assets/README.md` |
| Change where the client looks for `assets/` | `findAssetFolder()` in `src/client/Assets/AssetFolder.cpp` |
| Show a new image in the game | `assets/sprites/`, the `AssetList` of the screen, a `Sprite` component on the entity |
| Load a new kind of asset | `AssetKind`, `AssetList` and `AssetLibrary::load()` |
| Change what happens when an asset cannot be loaded | `AssetLoadingStep::loadNext()` and `AssetLoadingException` in `src/client/Exceptions/ClientException.hpp` |
| Read something only one system provides | A declaration in `src/client/Platform/`, one source file per system, chosen in its `CMakeLists.txt` |
| Change the number or the speed of the stars | `STAR_LAYERS` in `src/client/Rendering/Background/BackgroundConstants.hpp` |
| Change a color of the sky | `src/client/Rendering/Background/BackgroundColors.hpp` |
| Change how often sky events appear, or how many at once | `SHORTEST_SKY_EVENT_GAP`, `LONGEST_SKY_EVENT_GAP`, `MAXIMUM_SKY_EVENTS` in `src/client/Rendering/Background/SkyEvents/SkyEventConstants.hpp` |
| Add a kind of sky event | A class implementing `ISkyEvent` in `src/client/Rendering/Background/SkyEvents/`, a value of `SkyEventKind`, a weight in `SKY_EVENT_WEIGHTS`, a case in `SkyEventScheduler::createEvent` |
| Change the halo | `HALO_STANDARD_DEVIATION` in `BackgroundConstants.hpp`, `HALO_TINT` in `BackgroundColors.hpp`, the blur shader in `HaloShader.hpp`, `GaussianBlur/` |
| Change how the sky is enlarged | `ScrollingBackground::draw`, `IntegerScale/` |
| Change the window or the frame loop | `src/client/Window/GameWindow/` |
| Add a client error | `src/client/Exceptions/ClientException.hpp` |

Each folder has its own `CMakeLists.txt` declaring one library; `src/client/Rendering/CMakeLists.txt` adds `Background/`.
