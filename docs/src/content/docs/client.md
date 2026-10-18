---
title: Client
description: What the client does, how its parts fit together and where to find them in the code.
---

The client, `r-type_client` (`src/client/`), shows the game, plays its sounds and reads the player's input; the server decides what happens in the match (see [Architecture](/R-Type/architecture/)). Today it finds its assets folder, loads the assets of its interface, opens a window and runs its screens over a scrolling starfield: the title screen it opens on, a connection screen, a loading screen, the game screen, which draws its entities from one sprite sheet (for now a provisional showcase), an options screen and a quit confirmation, the menus all built from the same widgets. The game screen reads the player's actions through the keys the player chose, the arrows and the space bar at first; the options change them, with the volumes, the window size and the accessibility settings, and save them in a file for the next launch. It reads the server's address and port and the player's nickname at launch, shows them on its connection screen, and joins the server from there: it sends the connection request of the protocol, on a network thread opened at launch, until the server accepts or refuses it. Once accepted, it opens the local game: nothing the server sends after the acceptance is read yet, so the entities the server announces, which the client can apply, never reach it.

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Window | Available | `src/client/Window/` |
| Frame loop and screen stack | Available | `src/client/Application/` |
| Screens | Available: title, connection, loading, game, options and quit confirmation | `src/client/Screens/` |
| Menu widgets and their builder | Available; used by the title, the connection screen, the options and the quit confirmation | `src/client/Widgets/` |
| Text field | Available; used by the connection screen | `src/client/Widgets/TextField/` |
| Controls of a row: slider, switch, selector, list, key slot, link | Available; used by the options | `src/client/Widgets/RowControls/` |
| Assets folder and asset ids | Available | `src/client/Assets/`, `assets/README.md` |
| Asset loading | Available; used at launch for the interface assets, and by the loading screen | `src/client/Assets/AssetLibrary/`, `src/client/Assets/AssetLoadingStep/` |
| Path of the running executable, the reduced motion setting of the system, the settings folder | Available | `src/client/Platform/` |
| Player settings and their file | Available; the volumes, the color mode, the flashes, the sound cues and auto-fire are saved, but nothing reads them yet | `src/client/Settings/` |
| Drawing entities | Available; drawn by the game screen | `src/client/Rendering/EntityLayer/`, `src/client/Rendering/RenderSystem/`, `src/client/Rendering/SfmlDrawSurface/`, `src/client/Rendering/AssetTextureSource/` |
| Sprites of the ships, Bydos and missiles | Available; first animation frame only | `assets/sprites/entities.png`, `src/client/Rendering/EntitySprites.cpp` |
| Scrolling background | Available; drawn by the frame loop under every screen | `src/client/Rendering/Background/` |
| Pictures of the menus, pixel fonts | Available | `src/client/Rendering/InterfaceCanvas/`, `assets/fonts/` |
| Squadron of the title screen | Available | `src/client/Rendering/Squadron/` |
| Player commands | Available; read by the game screen, not sent to the server yet | `src/client/Input/` |
| Key remapping | Available; set in the options, read by the game screen | `src/client/Input/` |
| Network thread | Available; opened at launch | `src/client/Connection/NetworkThread/` |
| Ignoring an old world state | Available; used by `EntityReplicator` | `src/client/Replication/WorldStateFilter/` |
| Applying the entities the server announces | Available; nothing in the client calls it yet | `src/client/Replication/EntityReplicator/` |
| Server address, port and nickname at launch | Available; they fill the connection screen | `src/client/LaunchOptions/` |
| Joining a server | Available: request, challenge, acceptance or refusal; nothing is read after the acceptance yet | `src/client/Connection/ServerConnection/` |
| Sounds and music of the game | Not yet written | — |

## Frame loop and screens: `Application`, `ScreenStack`

`Application` (`src/client/Application/Application/`) opens the window, `GameWindow`, and runs the frame loop until the window is closed or no screen is left. A screen implements `IScreen` (`src/client/Screens/`): `handleEvent()`, `update(elapsed)`, `draw()`, in playfield units, and `hidesScreensBelow()`; `elapsed` is the real time since the previous frame, the same for every screen and for the scrolling background. Screens live in a `ScreenStack` (`src/client/Application/ScreenStack/`). Each frame, the window events go to the screen on top only, then every screen is updated, so a screen pushed over the game leaves the game running behind it. Screens are drawn from the bottom up, starting at the topmost screen whose `hidesScreensBelow()` is true: the screens under it are updated but not drawn, and they receive no event, so a click on the options never reaches a button of the title. The title, the connection screen, the loading screen, the game and the options over the title hide the screens below; the quit confirmation and the options over a game do not, so the screen under them shows through their dimmed layer. `GameWindow` handles closing and resizing itself and never passes those events on.

Screens are asked for by identifier: `push()` and `replaceAll()` take a `ScreenId` (`src/client/Screens/ScreenId.hpp`), so a screen never builds the screens it opens. `Application::createScreen()` is the one list of every screen, a `switch` with one case per `ScreenId`; a value without a case does not compile. `push()`, `pop()`, `replaceAll()` and `closeAll()` only ask for a change; the loop applies the changes after each event and after the update, in the order they were asked for. A screen may therefore pop itself while it handles an event, and the next event already reaches the new screen on top. A screen sees the stack only as an `IScreenNavigator` (`src/client/Screens/IScreenNavigator.hpp`), which holds `push()`, `pop()`, `replaceAll()` and `closeAll()` and nothing else: applying the changes, updating, drawing and handing out events stay with the frame loop, so no screen can break this order. A screen is built when its change is applied: `replaceAll()` destroys every screen, from the top down, before building the new one; `closeAll()` destroys them all and builds none, so the frame loop stops and the client closes. Building the game, or its loading screen, releases the assets of a previous game, so `ScreenId::Game` is always entered through `replaceAll()`, never pushed over a screen that still draws them.

| Screen | Shows | Leaves the stack when |
|---|---|---|
| `TitleScreen` | the logo, PLAY, SETTINGS and QUIT, and the squadron, over the scrolling background; PLAY pushes the connection screen over it | the connection screen asks `replaceAll(ScreenId::Game)` |
| `ConnectionScreen` | over the title, which it hides: a form centered on the picture, DIRECT CONNECT, the fields ADDRESS, PORT and PILOT, CONNECT and BACK, then a status line | BACK or Escape drops the attempt and closes it; once the server has accepted the player for `ACCEPTED_STATUS_DURATION` (1 s), it asks `replaceAll(ScreenId::Game)` |
| `LoadingScreen` | a progress bar; one file is loaded per frame | every file is loaded: it asks again, with `replaceAll()`, for the screen it loads for, which `createScreen()` now builds itself |
| `GameScreen` | its entities, over the scrolling background (today the provisional sprite showcase); releasing `OPTIONS_KEY` (Escape) releases what the player holds and pushes the options over it (`ScreenId::GameOptions`) | not yet: the end of a game is not written |
| `OptionsScreen` | over the title, which it hides (`ScreenId::Options`), or over the game, which goes on running behind it, dimmed (`ScreenId::GameOptions`): SETTINGS, the tabs SOUND, CONTROLS, DISPLAY and ACCESS, a panel of rows, then BACK (see [Options](/R-Type/client/#options-optionsscreen)) | BACK, or Escape (`MENU_CANCEL_KEY`) once no list is open and no key is awaited, closes it |
| `QuitConfirmationScreen` | over the title, still drawn under a dimmed layer, a panel: ARE YOU SURE YOU WANT TO QUIT?, NO selected | NO or Escape closes it; YES asks `closeAll()`, which closes the client |

The frame loop draws the scrolling background under every screen, the loading screen and the options included, even when they hide the screens below; this is **provisional**, since the starfield will probably become the background of the game screen alone. At launch the client opens on the title screen, with `replaceAll(ScreenId::Title)`; PLAY pushes the connection screen, which asks for the game with `replaceAll(ScreenId::Game)` once a connection is accepted: the loading screen of `gameAssets()` while some are not loaded, then the game screen. A file the loading screen cannot load stops the client with exit code 1, naming every such file. The game opens the options on the release of `OPTIONS_KEY`, not on its press: the system repeats the press of a held key, which would open and close them again and again. Menus act on releases for the same reason (see [Menus](#menus-menubuilder-menu)).

## Assets: the folder and the ids

Every file the client reads at run time sits in `assets/`, at the root of the repository. At launch, `locateAssetFolder()` (`rtype_client_assets`) looks for `assets/` next to the executable, then in the folder above it, where the Visual Studio generator writes `Release/` and `Debug/`. The folder the client was launched from plays no part. When neither folder exists, the client stops with exit code 1 and an `AssetFolderNotFoundException` naming both folders it searched.

Code names a file by its **asset id**: its path in `assets/`, with `/` between folders, for example `sprites/player_ship.png`. Every folder and file name of an id starts with a lowercase letter or a digit, then holds only `a-z`, digits, `_`, `-` and `.`; `isValidAssetId()` checks it. An id is therefore spelled the same on every system and never names a file outside `assets/`, and a later mod folder can replace a file by holding the same path. Ids are written once, as named constants.

## Assets: loading

`AssetLibrary` (`rtype_client_asset_library`) owns every loaded asset. `load(kind, id)` reads a texture, a sound or a font the first time only, and only locates a music file, which is streamed while it plays. It never throws: it returns an `AssetLoadResult` (`Loaded`, `InvalidId`, `MissingFile`, `UnreadableFile`). Its lookups, `texture()`, `sound()`, `font()` and `musicFile()`, never read the disk; an id that is not loaded throws `AssetNotLoadedException`. `keepPermanently()` makes assets permanent: no release drops them. A reference stays valid until its asset is released, so a sprite, which keeps a reference to its texture, must be gone before its texture is released.

`AssetLoadingStep` (`rtype_client_asset_loading_step`) loads an `AssetList`. When it is created, it releases every loaded asset the list does not name, except the permanent ones, so whatever still uses them must already be destroyed; the assets already loaded are neither read again nor counted. Then `loadNext()` loads one asset per call, so a loading screen can be drawn between two files. After the last asset, if any could not be loaded, it throws one `AssetLoadingException` naming every invalid id, missing file and unreadable file.

Assets are loaded at three moments only, all declared in `src/client/Screens/ScreenAssets.hpp` and listed in `ScreenAssets.cpp`:

- at launch, before the window opens, `loadInterfaceAssets()` loads `interfaceAssets()`, the assets of the interface, which any screen may use at any time, in a game or outside one (today the two pixel fonts of the menus, `REGULAR_FONT_ID` and `SMALL_FONT_ID`), and makes them permanent, so changing screen outside a game never reads a font;
- right after, still before the window opens, `loadTitleAssets()` loads `titleAssets()`: the logo (`LOGO_ID`, `sprites/logo.png`) and the entity sheet the squadron is drawn from, so the first frame already shows the title. They are not permanent: entering a game releases the logo, and a title screen built later goes through the loading screen of `titleAssets()` when one is missing;
- when the player enters a game (`ScreenId::Game`), the loading screen loads `gameAssets()`, one file per frame, after releasing what a previous game used and this one does not. When they are all loaded already, the game screen is built at once, after the same release. Today `gameAssets()` holds the entity sheet (`ENTITY_SHEET_ID`) alone, which the title already loaded: PLAY builds the game screen at once, and the loading screen is never seen.

### What a loading step does

The loading screen creates the step, then calls `loadNext()` once per frame until `isFinished()`, drawing the progress in between. At launch, `loadInterfaceAssets()` runs a step to its end at once, before the window opens.

```mermaid
sequenceDiagram
  accTitle: Loading the assets of a game
  accDescr: The loading screen creates a loading step with the asset list of the game. The step asks the asset library to release what the list does not name, except the permanent assets. Then, once per frame, the loading screen asks the step for the next asset of the list that was not loaded when the step was created; the step asks the library to load it, the library reads the file with SFML, and returns how it went. After the last asset the step is finished, or throws one error naming every problem.
  participant Screen as LoadingScreen
  participant Step as AssetLoadingStep
  participant Library as AssetLibrary
  participant Sfml as SFML
  Screen->>Step: create(list)
  Step->>Library: release unlisted, not permanent
  loop once per frame
    Screen->>Step: loadNext()
    Step->>Library: load(kind, id)
    Library->>Sfml: read file
    Sfml-->>Library: asset
    Library-->>Step: result
  end
  Step-->>Screen: done, or error
```

## Assets in the game: from an entity to the screen

The engine knows nothing about assets, and the game only names them: an entity that is drawn carries a `Sprite` component (`rtype_game`) holding an asset id, the rectangle of that image to show (`SpriteFrame`, in pixels of the image) and a layer. Each frame, `RenderSystem` (`src/client/Rendering/`) walks the entities that have a `Position` and a `Sprite`, layer by layer, and asks an `IDrawSurface` to draw each rectangle at its position. `SfmlDrawSurface` finds the texture through `ITextureSource`, which `AssetTextureSource` reads from the `AssetLibrary` without loading anything; an id with no loaded texture is skipped and logged once.

`SfmlDrawSurface` paints on a `PixelCanvas`, the pixel grid of the sky (see [How it is drawn](#how-it-is-drawn)): a position becomes a pixel of the canvas, which covers `BACKGROUND_PIXEL_SIZE` (4) logical units, and each rectangle lands on whole pixels, so a sprite is copied pixel for pixel and its pixels match the stars at every window size. An entity therefore moves on screen by whole background pixels, 4 logical units at a time.

The canvas belongs to `EntityLayer` (`src/client/Rendering/EntityLayer/`), behind the `IEntityLayer` interface. The game screen owns the entities of the game, which appear and disappear with it, and an entity layer. It paints the layer at construction and in every `update()`: the layer clears its canvas to transparent, lets `RenderSystem` paint on it, and ends the picture. In `draw()` it only lays the layer, since `draw()` is const; the entities sit over the background and under any screen pushed over the game. `GameScreen` sees only `IEntityLayer`, so `client_screen_tests` gives it a recording double and needs no graphics card.

```mermaid
flowchart TB
  accTitle: From the game screen to the screen
  accDescr: The game screen asks its entity layer to paint its entities. The layer runs RenderSystem, which passes each sprite to SfmlDrawSurface; the surface finds the texture through AssetTextureSource, reading the asset library, and paints the rectangle on the pixel canvas, which the layer lays over the sky.
  Game[GameScreen] --> Layer[EntityLayer]
  Layer --> Render[RenderSystem]
  Render --> Surface[SfmlDrawSurface]
  Surface --> Source[AssetTextureSource]
  Source --> Library[AssetLibrary]
  Surface --> Canvas[PixelCanvas]
```

Showing a new image takes four steps:

1. Draw it at its native size into `assets/sprites/entities.png`, or put a new file in `assets/sprites/`, named after the asset id rule.
2. Write its rectangle once, as a named constant (`EntitySpriteConstants.hpp` for the entity sheet), and the id of a new file too.
3. List the id in `gameAssets()` (`src/client/Screens/ScreenAssets.cpp`), which already names the entity sheet.
4. Give the entity a `Sprite` with that id, rectangle and layer: for an entity type, a case in `spriteOf`.

## Entity sprites: `spriteOf`

`spriteOf(type)` (`rtype_client_entity_sprites`) is the single table that gives each `EntityType` its `Sprite`: the entity sheet `sprites/entities.png` (`ENTITY_SHEET_ID`), the rectangle of the type, and the `Entities` layer. A value outside `EntityType` gets no sprite, never a neighbor's. `EntityReplicator` calls it once per entity it creates, after turning the type number of the protocol into an `EntityType` (see [Entities from the server](#entities-from-the-server-entityreplicator)).

| Type | Sprite | Native size, in pixels | On the playfield, in logical units |
|---|---|---|---|
| `Player` | the ship of the first player color | 32 × 16 | 128 × 64 |
| `Enemy` | the Bydo | 24 × 24 | 96 × 96 |
| `PlayerMissile` | the player missile | 16 × 6 | 64 × 24 |
| `EnemyMissile` | the enemy missile | 8 × 8 | 32 × 32 |

The sheet and where it comes from are described in `assets/README.md`. Not yet done: the four players share the first ship color, though the sheet holds four; the bonus Bydo has no entity type; only the first of the two animation frames is shown. `client_rendering_tests` checks the table against the shipped sheet with `sf::Image`, which needs no graphics card: every rectangle lies inside the sheet, shows a visible pixel, cuts no sprite and overlaps no other type.

The entities on screen today are a **provisional** showcase: `spawnSpriteShowcase` (`src/client/Rendering/SpriteShowcase.hpp`) puts a player ship, a player missile, an enemy missile and a Bydo across the middle of the playfield when the game screen is built. It will be removed once the client creates the entities the server announces.

## Scrolling background: `ScrollingBackground`

`ScrollingBackground` (`rtype_client_scrolling_background`) fills the playfield behind everything else: three layers of stars drifting left at different speeds, a ringed planet, and random sky events. `Application`, the frame loop, moves it every frame by the real time elapsed since the previous frame, read from `engine::SystemClock`, so its speed does not depend on the frame rate, and draws it before the screens. A frame longer than `MAXIMUM_BACKGROUND_STEP` (50 ms) moves it by 50 ms only, so the sky does not jump after a stall. A star that leaves on the left comes back on the right at a new height, so the sky never shows a seam.

The sky is cosmetic and local to each client: it is not made of entities, nothing of it crosses the network, and each client draws a different sky (the seed comes from `std::random_device`).

```cpp
ScrollingBackground background(std::random_device{}());
background.advance(now - previousFrameTime);
background.draw(window);
```

`draw()` expects a target whose view shows the playfield in its logical units, as the render target of `GameWindow` is set up.

### What the sky holds

| Layer | Stars | Speed, in background pixels per second |
|---|---|---|
| Far | 441 | 4.5 |
| Middle | 90 | 12 |
| Near | 36 | 30 |

The planet drifts at `PLANET_SPEED` (1.8 background pixels per second), drawn in front of the far stars and behind the others. Sky events are shooting stars, flares, asteroids and comets: one starts every 0.8 to 2.6 seconds (`SHORTEST_SKY_EVENT_GAP`, `LONGEST_SKY_EVENT_GAP`), at most `MAXIMUM_SKY_EVENTS` (4) at once, and they animate `SKY_EVENT_CLOCK_RATE` (3) times faster than real time. The layers are `STAR_LAYERS` in `BackgroundConstants.hpp`; the events are in `SkyEvents/`.

### How it is drawn

The sky is painted on a `PixelCanvas` (`rtype_client_pixel_canvas`) of `BACKGROUND_SIZE`, 480 × 270 background pixels; a background pixel covers `BACKGROUND_PIXEL_SIZE` (4) logical units. A canvas is enlarged without smoothing by the largest whole factor that fits the playfield on screen (`integerScaleFactor`: 3 in a 1600 × 900 window), then smoothed over the remainder only, so its pixels stay square and equal at every window size. The entities are painted on a second `PixelCanvas`, laid over the sky, and each menu on its own (see [Menus](#menus-menubuilder-menu)). Painting with the default alpha blend on a canvas cleared to transparent stores colors already multiplied by their alpha, whether a pixel is opaque, transparent or translucent like the dimmed layer of a menu, so a canvas is laid with a premultiplied alpha blend: the smoothed edges of a sprite do not darken.

A halo is added on top of the sky: the sky blurred by a Gaussian of `HALO_STANDARD_DEVIATION` (3 background pixels), added at 70 % opacity (`HALO_TINT`). It comes from the sky only, so the sprites get none. On a machine without shaders, or whose driver does not compile the halo shader, the sky is drawn without halo and the reason is logged once by the `Background` module. A render texture that cannot be created raises `RenderTextureNotCreatedException`.

On a Retina Mac, SFML 3 renders every window at its size in points, not in screen pixels, and macOS scales the image up, so the whole client looks softer there than on other screens.

### Tested without a screen

`Starfield`, which moves and paints the sky, and the sky events paint through `IPixelSurface` and never touch the graphics card, so `client_background_tests` runs them on the CI. `PixelBatch` turns the painted pixels into one `sf::VertexArray` of points. Only `GaussianBlur`, `PixelCanvas`, `ScrollingBackground`, `InterfaceCanvas` and the drawing of entities (`EntityLayer`, `SfmlDrawSurface`, `AssetTextureSource`) need OpenGL, since a texture cannot exist without it; they are checked on screen.

## Menus: `MenuBuilder`, `Menu`

Every menu is made of the widgets of `src/client/Widgets/`: `Label`, a line of text, `Button`, a label and the action it runs, or a row and its control, and the pieces they are drawn with: `CutCornerBox`, the shape with cut corners of every bar and panel, `SunkBox`, a box sunk into its panel, `Outline`, solid or dashed, `CheckBox`, `KeyCap`, a key seen from the front, and `TabStrip`, the tabs of a tabbed panel. Each widget is a class in its own folder, with the types it owns nested in it (`Button::Text`, `Menu::Content`, `MenuBuilder::Frame`); the `Widgets/` folder itself holds the constants, colors and styles they share, the look of a bar in each state (`BarState`), the mark of a check box (`CheckMark`) and the text helpers of `WidgetText.hpp`: `regularTextWidth()`, the width of a line in the regular font, `centeredTextLeft()`, and `barTextColor()`, the color of the label of a bar in each state. A screen never places them. It tells a `MenuBuilder` what its menu holds, step by step, and `build()` places every part with the layout of its frame, then hands them to a `Menu`, which the screen keeps. The quit confirmation, in `QuitConfirmationScreen::buildMenu()`:

```cpp
return MenuBuilder(MenuBuilder::Frame::Dialog)
    .heading(CONFIRM_HEADING)
    .message(QUIT_QUESTION)
    .cancelButton({.label = NO_LABEL}, [&navigator] { navigator.pop(); })
    .button({.label = YES_LABEL}, [&navigator] { navigator.closeAll(); })
    .build();
```

| Frame | Layout |
|---|---|
| `Page` | a column against the left side of the safe margin (`SAFE_MARGIN`, 24 pixels): version, logo or heading, subtitle, a rule, the bars (`BUTTON_SIZE`, 160 × 20, `BUTTON_GAP` 8 apart; a text field is `FIELD_SIZE`, 208 × 26), then the description of the selected bar; an illustration against the right side of the margin, a footer in the bottom-right corner |
| `Dialog` | a panel (`DIALOG_SIZE`, 272 × 96) centered over a dimmed layer (`DIM_LAYER_COLOR`, the sky at 72 % opacity): heading, message, then the bars in a centered row |
| `Form` | a panel `FORM_WIDTH` (240) wide, as tall as its parts, centered over the sky: heading, the text fields one under the other (`FIELD_SIZE`, 208 × 26, `SPACE_BETWEEN_FIELDS` 4 apart), the bars in a centered row (`FORM_BUTTON_SIZE`, 80 × 20), then the description of the selected bar in small print and, `SPACE_BELOW_DESCRIPTION` (8) pixels under it, the status line; the pointer selects a bar only by clicking it |
| `TabbedPanel` | the heading and its rule as on a page; under the rule the tabs (`TAB_STRIP_TOP_LEFT`, (24, 50)) between the keycaps of `PREVIOUS_TAB_KEY` and `NEXT_TAB_KEY`; a panel of rows (`ROWS_PANEL_SIZE`, 432 × 150, at (24, 66); rows of `ROW_SIZE`, 424 × 16, `ROW_GAP` 2 apart); the bars under the panel (`PANEL_BAR_SIZE`, 80 × 20, at y `PANEL_BARS_TOP`, 224) and the description of the selected row on their right; an illustration in the panel under the rows, a notice in the top-right corner, the status line at the end of the rule (`STAMP_STYLE`); left and right step the value of the selected row |

Positions are in pixels of the 480 × 270 grid of the sky, one pixel covering 4 logical units (`WidgetConstants.hpp`). Those of a page and a dialog repeat the mockup of the title screen; those of a form repeat the picture of the connection screen the team chose, where the panel is 240 × 194 at (120, 38); those of a tabbed panel repeat the picture of the options the team chose. `backButton(navigator)` adds the one BACK button of the client, cancel button of its menu, which closes the screen on top.

### Keys and pointer

`Menu` holds the logic every menu shares; `client_interface_tests` checks it without a screen.

- The arrows (down and right to the next bar, up and left to the previous one) and Tab (Shift+Tab goes back) move the selection, wrapping around and skipping disabled bars. On a page or a dialog, the pointer selects the enabled bar it moves over; on a form it selects a bar only by clicking it, so the field the player types into never changes under a passing pointer (`Menu::Content::selectsOnHover`).
- Enter, or the left mouse button on a bar, presses it; once released (over the same bar for the mouse), the bar is activated as soon as it has looked pressed for `PRESS_DURATION` (140 ms). A row is pressed only when its control can be activated: Enter on a slider does nothing.
- Escape selects the cancel button and activates it at once.
- A release counts only when the menu saw the press, so the release of the key that opened a screen does nothing on it; losing the focus drops a press not released yet.
- On a row, a key other than Enter and Escape goes first to its control, and a left press over the control too; in a tabbed panel, Left and Right that the control does not use do nothing, since only Up and Down move between its rows (`Menu::Content::nextKeys` and `previousKeys`). While the control holds the input (a list open, a key awaited, a thumb dragged), every key, pointer and focus event goes to it alone, so Escape closes the list, not the menu. The tabs open on a click, or on the keys at the place of Q and E (`PREVIOUS_TAB_KEY`, `NEXT_TAB_KEY`), read by their place on the keyboard so they stay under the same fingers on AZERTY.
- On a text field, the characters typed and Backspace (`FIELD_ERASE_KEY`) go to the field while no bar is pressed; every other key keeps its meaning, so Enter runs the action of the field. The left mouse button selects a field without pressing it, so a click on a field never runs its action.

The selected bar fills with blue and its label turns dark, a change of fill that reads without color; a pressed bar drops its label by one pixel. An action must not destroy its own menu: screens ask for changes through their `IScreenNavigator`, which applies them later.

### Text fields: `TextField`

`TextField` (`rtype_client_text_field`, `src/client/Widgets/TextField/`) is text the player types: UTF-8, at most a number of bytes, and only the characters it accepts. A typed character goes at the end when the field accepts it and all its bytes fit, so the text never ends with half a character; erasing removes the last character whole; the caret stays at the end. `Characters::Printable` accepts Basic Latin from the space to the tilde and Latin-1 Supplement (U+00A0 to U+00FF), which both pixel fonts draw, 8 pixels wide in the regular font; `Digits` and `DigitsAndDots` narrow it to a port and an IPv4 address. No field ever holds a control character, such as the ones Enter, Tab and Backspace type along with their key. A field starts with a text typed through the same rules, so the characters it refuses are left out. `client_interface_tests` checks it without a screen.

A menu holds a field as one of its bars, drawn as the field: its label in small print, then a box sunk into the panel (`FIELD_BOX_SIZE`, 208 × 16, dark inside, its top and left sides in shade) holding the text. While the field is selected, its label and the four sides of its box turn cyan and a caret one pixel wide (`FIELD_CARET_SIZE`, 1 × 10) follows the text. A click on the label or the box selects the field. `textField()` adds one, and `Menu::fieldText()` reads it back by the order of its bar:

```cpp
MenuBuilder(MenuBuilder::Frame::Form)
    .textField({.label = PORT_LABEL, .description = PORT_DESCRIPTION},
               TextField(std::to_string(server.port), PORT_FIELD_SIZE,
                         TextField::Characters::Digits),
               connectAction)
```

A box shows `FIELD_CHARACTER_CAPACITY` (24) characters with the caret after them; nothing cuts a longer text, so a screen keeps the size of each of its fields, in bytes, within it, as `ConnectionScreenConstants.hpp` checks at compile time.

`Menu::setStatus()` sets a line written in gold (`STATUS_STYLE`) under the description of a form, empty for none; a page and a dialog write none.

### Controls of a row: `RowControl`

A row of a tabbed panel shows a `RowControl` (`src/client/Widgets/RowControls/`) on the right of its label: the value it shows and changes. A control keeps no copy of the value. It gets a `ValueBinding`, a function that reads and one that writes, so it always paints the current value, whoever changed it; it writes through `change()`, which writes only when the value differs, so the screen never saves a change that changes nothing. `width()` and `paint()` are all a control must write; every other method does nothing by default.

| Control | Shows | Keys |
|---|---|---|
| `Slider` | a sunk groove, the track filled up to a thumb, the value and its unit; `OFF` while inactive | Left and Right by a step, Home and End to the ends, Page Up and Page Down by a page; a press on the groove jumps to the nearest step, then the thumb follows the pointer until the release |
| `Switch` | OFF and ON side by side, the current one filled | Left sets off, Right sets on, Enter turns it over |
| `ChoiceSelector` | one choice of a few, `<` and `>` on the selected row | Left and Right step without wrapping, Enter steps to the next, wrapping around |
| `Dropdown` | one choice and a pointer down; open, the list under its box | Enter opens; open, Up and Down move the highlight past the choices that cannot be taken, Home and End, Enter takes, Escape or Tab close without a change |
| `KeySlot` | a keycap, gold with `!` when another action has the same key; `PRESS A KEY` while waiting | Enter waits for a key; the next key it accepts becomes the key, Escape cancels; the repeats of the key just taken are swallowed until its release |
| `LinkText` | a word and ` >` | Enter runs its action |

A screen adds a row with `MenuBuilder::row()`, as the options do for the music:

```cpp
builder.row({.label = MUSIC_LABEL},
            std::make_unique<Slider>(
                VOLUME_RANGE,
                ValueBinding<int>{
                    .read = [this] { return settings_->musicVolume; },
                    .write = [this](int volume) {
                      settings_->musicVolume = volume;
                      changed();
                    }},
                [this] { return settings_->muted; }, PERCENT_UNIT));
```

`client_row_control_tests` checks every control without a screen, and `client_widget_piece_tests` the pieces they are drawn with.

### How a menu is drawn: `InterfaceCanvas`

A menu paints through `IInterfaceCanvas` (`src/client/Rendering/`): rectangles, lines of text, and rectangles of a loaded texture cut to a visible area. `InterfaceCanvas` paints them on a `PixelCanvas` of its own, so menus have the square, sharp pixels of the sky. Text uses two pixel fonts under the SIL Open Font License, at the 8 pixels they are drawn for: Press Start 2P (`REGULAR_FONT_ID`) and Silkscreen (`SMALL_FONT_ID`, the small print). SFML leaves partly covered pixels on Silkscreen at that size, so text goes through a shader (`CrispTextShader.hpp`) that lights a pixel only when at least half of it is covered. Without shaders, text is drawn smoothed and the reason is logged once. A screen owns its canvas behind the interface, so its tests give it a recording double.

### The squadron: `Squadron`

`Squadron` (`src/client/Rendering/Squadron/`) plays on the right of the title screen, in the area its menu reserves. A line grows from the middle of the box, the box opens by 0.45 s, SQUADRON is written letter by letter, and the four ships, one per player color, fly in through the left edge and reach their places by 1.79 s. The formation then drifts in a figure eight of 14 × 10 pixels without leaving the box, and a random ship fires every 1.6 to 3 s. It plays again each time the title screen is built. When the system asks for less motion (`systemPrefersReducedMotion()`, read when the title is built), the box is open, the ships are at rest and none fires. The timings are in `SquadronConstants.hpp`.

## Connection screen: `ConnectionScreen`, `IServerConnection`

`ConnectionScreen` (`src/client/Screens/ConnectionScreen/`) shows the address, the port and the nickname given at launch in three fields, then CONNECT and BACK. Enter on a field or on CONNECT checks the fields before any attempt, and names the one to fix on the status line: `INVALID ADDRESS` for an address that is not four numbers (`ipv4AddressFromText`), `INVALID PORT` outside 1 to 65535 (`portFromText`), `ENTER A CALLSIGN` for an empty nickname. Otherwise it drops the attempt in progress and starts a new one. At each update the status line shows how the attempt stands: `CONNECTING...`, `CONNECTED AS PLAYER <n>`, `REFUSED: GAME FULL`, `REFUSED: GAME IN PROGRESS`, `REFUSED: OTHER VERSION` or `SERVER UNREACHABLE`. Once the server has accepted the player for `ACCEPTED_STATUS_DURATION` (1 s), the screen asks for the game, once; the lobby does not exist yet. Building the screen drops any earlier attempt, so reopening it never jumps to the game. Its texts and sizes are in `src/client/Screens/ConnectionScreenConstants.hpp`.

The screen drives an `IServerConnection` (`src/client/Connection/`): `connect(server, nickname)`, `cancel()` and `status()`, a `ConnectionStatus` whose `Phase` is `Idle`, `Connecting`, `Accepted` (with the player number), `Refused` (with its `network::RefusalReason`) or `Unreachable`. `Application` owns it, so it outlives every screen.

The connection behind it is a `ServerConnection` (see [Joining a server](#joining-a-server-serverconnection)), so PLAY leads to the game only through a running server: without one, the screen shows `SERVER UNREACHABLE` after `CONNECTION_ATTEMPT_TIMEOUT` (5 s).

## Joining a server: `ServerConnection`

`ServerConnection` (`rtype_client_server_connection`, `src/client/Connection/ServerConnection/`) implements `IServerConnection` with the connection of the protocol (see [Protocol](/R-Type/protocol/#connection)). `Application` owns it and calls `update(elapsed)` once per frame, before the screens: it reads what arrived and sends what is due, so the frame loop never waits on the server.

- `connect()` sends a `ConnectionRequest` without a cookie at once, and drops the answers left from an earlier attempt. While nothing answers, the request goes out again every `CONNECTION_REQUEST_INTERVAL` (500 ms).
- A `ConnectionChallenge` is answered at once with the request carrying its cookie; every later request carries it too.
- A `ConnectionAccepted` gives `Accepted` and the player number; a `ConnectionRefused` gives `Refused` and its reason. Either ends the attempt, and nothing more is sent.
- With neither, `CONNECTION_ATTEMPT_TIMEOUT` (5 s) after `connect()`, the attempt gives up as `Unreachable`; a challenge does not restart that delay, so a server that only challenges never holds the screen.
- Only datagrams from the server being contacted, of this protocol and version, and whose message sizes add up, are read; any other is ignored. `cancel()` stops the attempt where it stands.

It sends and receives through an `IDatagramLink` (`src/client/Connection/`): `NetworkThread` in the client, `RecordingDatagramLink` (`tests/doubles/`) in the tests. The session token of the acceptance is not kept yet: nothing is sent after it. Both delays are **provisional**, like every delay of the protocol page: code must read the constants of `src/client/Connection/ConnectionConstants.hpp`.

## Options: `OptionsScreen`

`OptionsScreen` (`src/client/Screens/OptionsScreen/`) shows the player's settings in four tabs, each a tabbed panel of rows over BACK:

| Tab | Rows |
|---|---|
| SOUND | MUSIC and EFFECTS, sliders from 0 to 100 % by `VOLUME_STEP` (5); MUTE ALL; DEFAULT SOUND |
| CONTROLS | MOVE UP, MOVE DOWN, MOVE LEFT, MOVE RIGHT, SHOOT and READY, one key slot each; DEFAULT KEYS |
| DISPLAY | WINDOW SIZE, a list, and under it every size of `WINDOW_SIZES` drawn to scale |
| ACCESS | COLOR MODE, a selector; REDUCE FLASHES, SOUND CUES, AUTO-FIRE |

Every row reads and writes the `PlayerSettings` that `Application` owns (see [Settings](/R-Type/client/#settings-playersettings-settingsfile)). A change takes effect at once: the window takes its new size, and the game reads the new keys as soon as the options close. It is saved at the next update, once a frame at most, so a slider dragged is saved at each step it crosses and a window closed meanwhile loses nothing. SAVED is then written at the end of the rule for `SAVED_STAMP_DURATION` (1 s); NOT SAVED stays until the next save that works. DEFAULT SOUND and DEFAULT KEYS ask first, in a dialog over the options with NO selected; Escape answers NO.

The description of the selected row follows its state: the keys of an open list, `> PRESS THE NEW KEY. ESC CANCELS` while a key is awaited, `> THIS KEY CANNOT BE BOUND` after a key that cannot be bound, `> SAME KEY AS SHOOT: CHANGE ONE` when two actions share a key, `> SOUND MUTED: SEE MUTE ALL` on a muted volume, and the sizes too large for the desktop, which the list shows dashed and never takes. Every description fits right of BACK, 43 characters at most.

The volumes, MUTE ALL, the color mode, REDUCE FLASHES, SOUND CUES, AUTO-FIRE and the READY key are saved and shown, but nothing reads them yet: there is no audio, no color palette, no flash or sound cue, no auto-fire and no lobby. Their descriptions end with `(COMING SOON)`.

Over the title (`ScreenId::Options`) the options hide it. Over a game (`ScreenId::GameOptions`) the game goes on running, dimmed under them, and THE GAME GOES ON is written in the top-right corner. The texts are in `src/client/Screens/OptionsScreenConstants.hpp`. The screen takes the window as an `ISizableWindow` (`src/client/Window/`) and saves through an `ISettingsStorage` (`src/client/Settings/`), so `client_screen_tests` checks it with a fake window and a recording storage from `tests/doubles/`.

The chart of the window sizes is `paintWindowSizeChart()` (`rtype_client_window_size_chart`, `src/client/Widgets/WindowSizeChart.cpp`): one pixel for `WINDOW_SIZE_CHART_SCALE` (20) pixels of a window, every size centered on the same point, the size of the window in cyan, the size highlighted in the open list in gold, the sizes too large dashed, and a legend on the right (NOW, NEW, TOO LARGE).

## Settings: `PlayerSettings`, `SettingsFile`

`PlayerSettings` (`rtype_client_settings`, `src/client/Settings/`) holds what the player sets: the music and effects volumes, in percent from `MINIMUM_VOLUME` (0) to `MAXIMUM_VOLUME` (100), mute, the window size, empty until the player picks one, the keys (`KeyBindings`), the color mode, reduce flashes, sound cues and auto-fire. A default-built value holds the settings of a first launch; `Application` then takes reduce flashes from the system (`systemPrefersReducedMotion()`).

`SettingsFile` (`rtype_client_settings_file`, `src/client/Settings/SettingsFile/`) keeps them in a text file, one `name=value` per line, readable and editable by hand. A first launch writes:

```ini
# R-Type settings, written by the game
version=1
music_volume=70
effects_volume=80
mute_all=off
key_move_up=UP
key_move_down=DOWN
key_move_left=LEFT
key_move_right=RIGHT
key_shoot=SPACE
key_ready=R
color_mode=off
reduce_flashes=off
sound_cues=off
auto_fire=off
```

`window_size=1920x1080` follows `mute_all` once a size is picked. `readSettingsText()` reads a file over the settings of a first launch: a line that is missing, unknown or invalid leaves the default of that setting only, and is logged as a warning with its number (`line 3: unknown setting`); spaces around a name or a value, a final `\r` and lines starting with `#` are ignored; a file larger than `MAXIMUM_SETTINGS_FILE_SIZE` (65536 bytes) is ignored whole. A save writes `settings.ini.tmp` next to the file, then renames it over the file, so the file is always the previous one or the new one. The folder is created on the first save, open to its owner only.

| System | Folder of `settings.ini` |
|---|---|
| Linux | `$XDG_CONFIG_HOME/r-type`, or `$HOME/.config/r-type` when that variable is unset, empty or not absolute |
| macOS | `~/Library/Application Support/R-Type` |
| Windows | `%LOCALAPPDATA%\R-Type` |

`settingsFolder()` (`src/client/Platform/`) finds it, and the client logs `settings file: <path>` at launch; when the system tells no folder, the settings last for that launch only. A size saved that no longer fits the desktop is kept in the file, and the window opens at the largest size that fits.

## Player commands: `PlayerCommands`

The game never sees keys, only **actions**. `PlayerAction` (`src/client/Input/`) has five values, and each one is its bit in the `actions` byte of `PlayerInput` (see [Protocol](/R-Type/protocol/#inputs)): 0 up, 1 down, 2 left, 3 right, 4 shoot. `ActionSet` (`rtype_client_action_set`) holds the actions held at a moment, and `bits()` returns that byte: 0 when nothing is held, never a bit above 4.

`KeyBindings` (`rtype_client_key_bindings`) is the only place that knows which key does what, for six bound actions (`BoundAction`, `src/client/Input/BoundAction.hpp`): the four moves and the shot, which are player actions, and ready, for the lobby. A binding is a scancode, the place of a key on the keyboard and not its character, so the keys of a first launch (`DEFAULT_KEYS`: the arrows, space and R) fall under the same fingers on every layout. The player changes them in the options. Two actions may share a key: the options flag it, and in game the first of them in the order of `BOUND_ACTIONS` takes the key.

Only the 105 keys of `KEY_NAMES` (`src/client/Input/InputConstants.hpp`) can be bound, each with the name the settings file writes (`SPACE`, `PAD 1`, `LEFT ALT`): SFML has no name that stays the same across systems and layouts, so the client keeps its own. Escape is left out, since it opens and closes the menus, and so are the Command keys of a Mac, which are the Windows keys of a PC: their system shortcuts (⌘W closes the window, ⌘Q quits) would leave the game. A keycap shows the letter the layout prints on a letter key (`localizedKeyName()`), so the place of Q reads A on AZERTY, and the name of `KEY_NAMES` on any other key.

`PlayerCommands` (`rtype_client_player_commands`) applies the key and focus events of the window to an `ActionSet`, through the `KeyBindings` the player chose, read at each event. While the window has no focus it ignores keys, and losing the focus releases everything: the release of a key held at that moment goes to another window, so it would stay held for good. A key still held when the focus comes back counts only once it is pressed again. Left and right held together give both bits; the receiver decides.

```cpp
const ActionSet held = gameScreen.playerActions();
const std::uint8_t actions = held.bits();
```

`GameScreen::playerActions()` is the only way out, since the window hands the key and focus events to the screen on top. Nothing reads it yet: there is no player ship on the client and no `PlayerInput` is sent. The game screen logs `actions held: <byte>` at trace level each time the byte changes. It assumes the window has the focus when it is built and learns otherwise from the focus events. Only the screen on top receives events, so opening the options releases every action held (`PlayerCommands::releaseAll()`): a key changed in the options would otherwise never release its action.

## Launch options: `LaunchOptions`

`LaunchOptions` (`src/client/LaunchOptions/`) reads the address and the port of the server and the nickname of the player, given at launch, from the arguments the logging options left (see [Logging](/R-Type/logging/#choosing-the-level-and-the-output-at-launch)), with the engine's `readLaunchArguments` (see [Engine](/R-Type/engine/#launch-arguments-readlauncharguments)):

| Option | Effect |
|---|---|
| `-h <address>` | The IPv4 address of the server, four numbers such as `127.0.0.1` (see [Network](/R-Type/network/#ports-and-addresses-written-as-text-portfromtext-ipv4addressfromtext)). Default: `DEFAULT_SERVER_ADDRESS` (`127.0.0.1`), a server on the same machine. |
| `-p <port>` | The UDP port of the server, from 1 to 65535. Default: `DEFAULT_SERVER_PORT` (4242). |
| `-n <nickname>` | The nickname of the player, 1 to 16 bytes of UTF-8 (`MAXIMUM_NICKNAME_SIZE`, see [Protocol](/R-Type/protocol/#connection)). Default: `DEFAULT_NICKNAME` (`PILOT`). The connection screen leaves out the characters its field refuses. |
| `--help` | Prints the usage, the logging options included, on standard output, then exits with code 0 without opening the window or the journal. |

- A host name such as `localhost` is refused, not resolved: a resolution can block for seconds, so it must run on the network thread once the connection exists. An IPv6 address is refused too, since the server only listens over IPv4.
- The last of each option wins. An unknown argument, an option without its value, an invalid address, an invalid port or an invalid nickname (`InvalidNicknameOptionException`) stops the client before the window opens: it prints `r-type_client:`, the reason and how to see the usage on standard error, then exits with code 1.
- `main()` logs `server: <address> port <port>` at the `info` level and hands the server and the nickname to `Application`, which starts the connection screen with them. CONNECT on that screen joins the server (see [Joining a server](#joining-a-server-serverconnection)).
- On Windows, the arguments are not passed in UTF-8, so a nickname with an accent may be refused at launch; it can be typed on the connection screen instead.

```bash
./r-type_client -h 192.168.1.20 -p 4243 -n ACE
```

## Network: `NetworkThread`

`NetworkThread` (`rtype_client_network_thread`, `src/client/Connection/NetworkThread/`) runs the client's UDP socket on a thread of its own, so the frame loop never waits on the network. It is built from the parts of the network library: a `UdpSocket` on its `NetworkContext`, and a `DatagramRelay` between the socket and two `BoundedQueue` (see [Network](/R-Type/network/#socket-to-queues-datagramrelay)). The server uses the same socket and context, but sorts what it receives before queueing it, and uses the relay only to send.

- The constructor opens the socket on an `Endpoint` (port 0 lets the system pick a free port) and starts the thread. The destructor stops the thread and waits for it; a datagram not sent yet is lost.
- The network thread copies every datagram received, with its sender, into the incoming queue. `drainReceived()` moves them, oldest first, to the end of a vector, and returns at once when nothing arrived: `ServerConnection::update()` calls it once per frame.
- `send()` puts an `OutgoingDatagram` in the outgoing queue, asks the network thread to send what waits there through `NetworkContext::post()`, and returns at once.
- An exception on the network thread, such as a datagram of more than `MAX_DATAGRAM_SIZE` (1200) bytes, stops it, and every later `drainReceived()` throws it.
- Every call is safe from any thread.

The incoming queue holds `INCOMING_DATAGRAM_QUEUE_CAPACITY` (256) datagrams, nearly thirteen seconds of the 20 world states per second of the protocol; the outgoing queue holds `OUTGOING_DATAGRAM_QUEUE_CAPACITY` (64), about one second of the 60 inputs per second the client sends (`src/client/Connection/ConnectionConstants.hpp`). These sizes are **provisional**: they will be revised with the rates of the protocol, and code must read the constants. When a queue is full, its oldest datagram is discarded; nothing reports it yet.

`Application` opens one at launch, on every IPv4 interface (`ANY_IPV4_ADDRESS`) and a port the system picks (`ANY_PORT`), and the connection to a server sends and receives through it, as an `IDatagramLink` (see [Joining a server](#joining-a-server-serverconnection)). A socket the system refuses to open stops the client: `main()` logs and prints `client stopped:` with the reason, and exits with code 1.

## Platform: the executable path

`executablePath()` (`src/client/Platform/`) returns the path of the running executable, or nothing when the system does not tell. Each system has its own source file, `ExecutablePathLinux.cpp`, `ExecutablePathMacOs.cpp` and `ExecutablePathWindows.cpp`, and CMake builds only the one that matches.

`systemPrefersReducedMotion()`, in the same folder and the same way, tells whether the player asked the system for less motion: "Reduce motion" on macOS (read from AppKit), "Show animations in Windows" turned off on Windows (`SPI_GETCLIENTAREAANIMATION`), the GNOME setting `enable-animations` turned off on Linux (read with `gsettings`). A setting the system does not give counts as no request. No test reads it: its answer depends on the machine.

`settingsFolder()`, in the same folder and the same way, gives the folder where the client keeps the player's files (see [Settings](/R-Type/client/#settings-playersettings-settingsfile)), or nothing when the system does not tell.

## World state: `WorldStateFilter`

UDP may deliver a datagram sent earlier after one sent later, and applying the old one would move entities backwards. `WorldStateFilter` (`src/client/Replication/WorldStateFilter/`) remembers the highest `tick` it has accepted. `accept(tick)` returns `false` when `tick` is lower than that, and the state must be dropped; otherwise it returns `true` and remembers the tick. An equal tick is accepted, because it is another part of the same state (see [Protocol](/R-Type/protocol/#world-state)).

- The first state is always accepted, and a refused state does not lower the remembered tick.
- Ticks are compared as plain integers, with no wrap-around: a 32-bit tick lasts 2.27 years at 60 ticks per second.
- Call it only for datagrams that passed the checks on the sender, otherwise a forged datagram could raise the remembered tick and make every real state look old. `ServerConnection` checks the sender address, the protocol id, the version and the flags of a datagram, but not its session token yet, so a datagram forged with the server's address still passes.
- `EntityReplicator` calls it for every `WorldState` it is given (see below); nothing in the client gives it states yet.

```cpp
const rtype::network::WorldState state =
    rtype::network::WorldStateCodec::decode(datagram.payload);
if (!worldStateFilter.accept(state.tick)) {
  return;
}
```

## Entities from the server: `EntityReplicator`

`EntityReplicator` (`rtype_client_entity_replicator`, `src/client/Replication/EntityReplicator/`) applies the entities the server announces to the registries of the game screen, so rendering only draws what the registries hold. It runs no rule of the game: where an entity is and when it disappears come from the server.

- `apply(EntitySpawned)` creates the entity, unless the client already has it. A created entity gets a `Position`, in logical units, and the `Sprite` that `spriteOf` gives its type, and nothing else.
- `apply(WorldState)` moves every entity the state lists and creates those the client does not have, as after a lost `EntitySpawned`. A state older than the newest one applied is ignored (`WorldStateFilter`); an equal tick is another part of the same state and is applied.
- `apply(EntityDestroyed)` destroys the entity it names; an id the client does not have changes nothing, so the later copies do nothing. An entity destroyed, then listed by a `WorldState` that arrives late, is created again, as the protocol asks; the next copy of the `EntityDestroyed` removes it.
- A message naming `NO_ENTITY_ID` or a type outside `WireEntityType` creates nothing.

It binds the network id of each entity it creates in a `NetworkEntityTable`, so its entities must be destroyed through it. One replicator serves one game, because the ticks start again with each game. It is not thread-safe: the frame loop calls it, with messages whose sender passed the checks.

It does not yet remove an entity that no `WorldState` has listed for 900 ms (see [Protocol](/R-Type/protocol/#losses)): until then, an entity whose three `EntityDestroyed` were all lost stays on screen. It does not read the players of a `WorldState`, and plays no appearance or explosion effect. Nothing in the client calls it yet: `ServerConnection` reads nothing the server sends after the acceptance (see [Joining a server](#joining-a-server-serverconnection)). It is meant to be registered on the `MessageDispatcher` of the connection, once per message type:

```cpp
rtype::client::EntityReplicator replicator{entities, components};
dispatcher.registerHandler<rtype::network::WorldState>(
    [&replicator](const rtype::network::MessageOrigin&,
                  const rtype::network::WorldState& state) {
      replicator.apply(state);
    });
```

## Where to intervene

| You want to… | Look at |
|---|---|
| Change which world states the client ignores | `WorldStateFilter::accept` in `src/client/Replication/WorldStateFilter/` |
| Change how the client applies the entities the server announces | `EntityReplicator` in `src/client/Replication/EntityReplicator/` |
| Add an entity type to the protocol | `entityTypeOf` in `src/client/Replication/EntityReplicator/EntityReplicator.cpp` and `toWireEntityType` on the server, then `spriteOf` for its look |
| Add an asset | A file in `assets/sprites/`, `sounds/`, `music/` or `fonts/`, and its id in `gameAssets()`, in `titleAssets()` for the title screen, or in `interfaceAssets()` for a font any screen may use (`src/client/Screens/ScreenAssets.cpp`); `client_screen_tests` loads `interfaceAssets()` for real, and the CI cannot create a texture |
| Change the asset id rule | `isValidAssetId()` in `src/client/Assets/AssetIds.cpp`, `ASSET_ID_PUNCTUATION` in `AssetConstants.hpp`, and `assets/README.md` |
| Change where the client looks for `assets/` | `findAssetFolder()` in `src/client/Assets/AssetFolder.cpp` |
| Show a new image in the game | `assets/sprites/`, its rectangle in `src/client/Rendering/EntitySpriteConstants.hpp`, `gameAssets()`, `spriteOf` in `src/client/Rendering/EntitySprites.cpp` |
| Change how an entity type looks | `spriteOf` in `src/client/Rendering/EntitySprites.cpp`, the rectangles in `EntitySpriteConstants.hpp`, `assets/sprites/entities.png` |
| Change how entities are placed on the pixel grid | `SfmlDrawSurface::draw` in `src/client/Rendering/SfmlDrawSurface/` |
| Change when the entities are painted and laid | `GameScreen::update()` and `GameScreen::draw()` in `src/client/Screens/GameScreen/`, `EntityLayer` in `src/client/Rendering/EntityLayer/` |
| Remove the sprite showcase | `SpriteShowcase.hpp`, `SpriteShowcase.cpp` and `SpriteShowcaseConstants.hpp` in `src/client/Rendering/`, the call in the constructor of `GameScreen` |
| Add a screen | A value of `ScreenId` (`src/client/Screens/ScreenId.hpp`) and its case in `Application::createScreen()` and in `nameOf()` of `tests/client/Screens/RecordingScreen.cpp`; a class implementing `IScreen` in its own folder under `src/client/Screens/`, whose `hidesScreensBelow()` says whether the screens under it stay drawn, taking an `IScreenNavigator` if it opens or closes screens, with its `CMakeLists.txt` target and `add_subdirectory` in `src/client/Screens/CMakeLists.txt`; the link in `src/client/Application/Application/CMakeLists.txt` |
| Add a menu | A screen that builds a `Menu` with `MenuBuilder` (`src/client/Widgets/`), as `QuitConfirmationScreen` does |
| Add a text field to a menu | `MenuBuilder::textField()` with a `TextField` (`src/client/Widgets/TextField/`), then `Menu::fieldText()`, as `ConnectionScreen` does |
| Change the characters a field accepts | `TextField::Characters` and `TextField::accepts()`; the ranges in `src/client/Widgets/WidgetConstants.hpp` |
| Change how the client joins a server | `ServerConnection` in `src/client/Connection/ServerConnection/`; `CONNECTION_REQUEST_INTERVAL` and `CONNECTION_ATTEMPT_TIMEOUT` in `src/client/Connection/ConnectionConstants.hpp` |
| Change the texts, sizes or checks of the connection screen | `src/client/Screens/ConnectionScreenConstants.hpp`, `ConnectionScreen::connect()` |
| Change where the parts of the menus go | `MenuBuilder::placeColumn()`, `placeDialog()`, `placeForm()` and `placeTabbedPanel()`, `src/client/Widgets/WidgetConstants.hpp` |
| Add a control to a row | A class deriving from `RowControl` in its own folder under `src/client/Widgets/RowControls/`, its sizes in `RowControlConstants.hpp`, added with `MenuBuilder::row()` |
| Change the rows, texts or tabs of the options | `src/client/Screens/OptionsScreenConstants.hpp`, `OptionsScreen::soundTab()`, `controlsTab()`, `displayTab()` and `accessTab()` in `src/client/Screens/OptionsScreen/` |
| Add a setting | A member of `PlayerSettings`, its name in `src/client/Settings/SettingsConstants.hpp`, its line in `settingsText()` and `applyLine()` (`SettingsText.cpp`), its row in `OptionsScreen` |
| Change where the settings file is kept | `settingsFolder()` in `src/client/Platform/`, `SETTINGS_FILE_NAME` in `SettingsConstants.hpp` |
| Change how a bar looks | `Button::lookOf()`, `src/client/Widgets/WidgetColors.hpp` |
| Change the keys of the menus, or how long a press shows | `MENU_NEXT_KEYS`, `MENU_PREVIOUS_KEYS`, `MENU_TAB_KEY`, `MENU_CONFIRM_KEY`, `MENU_CANCEL_KEY`, `FIELD_ERASE_KEY`, `PRESS_DURATION` in `WidgetConstants.hpp` |
| Change the texts of the title screen or of the quit confirmation | `src/client/Screens/ScreenConstants.hpp` |
| Change the squadron | `src/client/Rendering/SquadronConstants.hpp` |
| Close the client from a screen | `closeAll()` of its `IScreenNavigator` |
| Open a screen from another | `push(ScreenId::…)` to put it over the current one, `replaceAll(ScreenId::…)` to replace every screen with it |
| Load a new kind of asset | `AssetKind`, `AssetList` and `AssetLibrary::load()` |
| Change what happens when an asset cannot be loaded | `AssetLoadingStep::loadNext()` and `AssetLoadingException` in `src/client/Exceptions/ClientException.hpp` |
| Read something only one system provides | A declaration in `src/client/Platform/`, one source file per system, chosen in its `CMakeLists.txt` |
| Change the number or the speed of the stars | `STAR_LAYERS` in `src/client/Rendering/Background/BackgroundConstants.hpp` |
| Change a color of the sky | `src/client/Rendering/Background/BackgroundColors.hpp` |
| Change how often sky events appear, or how many at once | `SHORTEST_SKY_EVENT_GAP`, `LONGEST_SKY_EVENT_GAP`, `MAXIMUM_SKY_EVENTS` in `src/client/Rendering/Background/SkyEvents/SkyEventConstants.hpp` |
| Add a kind of sky event | A class implementing `ISkyEvent` in `src/client/Rendering/Background/SkyEvents/`, a value of `SkyEventKind`, a weight in `SKY_EVENT_WEIGHTS`, a case in `SkyEventScheduler::createEvent` |
| Change the halo | `HALO_STANDARD_DEVIATION` in `BackgroundConstants.hpp`, `HALO_TINT` in `BackgroundColors.hpp`, the blur shader in `HaloShader.hpp`, `GaussianBlur/` |
| Change how the sky and the entities are enlarged | `PixelCanvas::show` in `src/client/Rendering/PixelCanvas/`, `IntegerScale/` in `src/client/Rendering/Background/` |
| Change the keys of a first launch, or which keys can be bound | `DEFAULT_KEYS` and `KEY_NAMES` in `src/client/Input/InputConstants.hpp` |
| Add a bound action | `BoundAction` and `BOUND_ACTIONS` (`src/client/Input/BoundAction.hpp`), `DEFAULT_KEYS`, `KeyBindings::playerActionOf()` when it is a `PlayerAction` with its bit in the protocol, `BOUND_ACTION_KEY_NAMES` in `SettingsConstants.hpp`, `BOUND_ACTION_LABELS` in `OptionsScreenConstants.hpp` |
| Change the window, or when the background moves and is drawn | `src/client/Window/GameWindow/`, `Application::measureFrame()` and `Application::render()` in `src/client/Application/Application/` |
| Change the default server address or port | `DEFAULT_SERVER_ADDRESS` in `src/client/ClientConstants.hpp`, `DEFAULT_SERVER_PORT` in `src/network/NetworkConstants.hpp` |
| Change the default nickname | `DEFAULT_NICKNAME` in `src/client/ClientConstants.hpp` |
| Add a launch option | The options listed in `LaunchOptions::read()` and `usage()`, in `src/client/LaunchOptions/LaunchOptions.cpp`; its name in `src/client/ClientConstants.hpp`, or in `src/engine/LaunchArguments/LaunchArgumentsConstants.hpp` when the server uses it too |
| Send or receive datagrams | `NetworkThread` in `src/client/Connection/NetworkThread/` |
| Change how many datagrams wait between the frame loop and the network thread | `INCOMING_DATAGRAM_QUEUE_CAPACITY` and `OUTGOING_DATAGRAM_QUEUE_CAPACITY` in `src/client/Connection/ConnectionConstants.hpp` |
| Test code that uses the network thread | `tests/client/Connection/NetworkThreadTest.cpp`: a peer socket on the loopback address |
| Test code that sends and receives without the network | `tests/doubles/RecordingDatagramLink.hpp`, as `tests/client/Connection/ServerConnectionTest.cpp` does |
| Add a client error | `src/client/Exceptions/ClientException.hpp` |

Each folder has its own `CMakeLists.txt` declaring one library; `src/client/Rendering/CMakeLists.txt` declares `rtype_client_entity_sprites` for the table and the showcase, and adds `RenderSystem/`, `SfmlDrawSurface/`, `AssetTextureSource/`, `PixelCanvas/`, `InterfaceCanvas/`, `Squadron/`, `EntityLayer/` and `Background/`.
