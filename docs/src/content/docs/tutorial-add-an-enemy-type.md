---
title: "Tutorial: add an enemy type"
description: A step-by-step walk from the behaviour of a new Bydo to its sheet, its tests and its appearance on screen, with the code and the data to write and the commands to check each step.
sidebar:
  label: "Tutorial: an enemy type"
---

This tutorial adds an enemy to the game, from the behaviour that moves it to the screen that shows it. It is written for someone who has not touched the C++ of this project yet: every step names the file, gives the code or the data and ends with a command that tells you it worked. Nothing here needs the help of the team.

The enemy is the **Zigzagger**: a Bydo that goes down, up and back to its row twice as fast as a [Weaver](/R-Type/waves-file/#enemies) weaves, along straight lines instead of a curve, and is worth 300 points. It is only an example: at the end you will have the steps to add your own, and you can keep your own enemy instead.

## Before you start

You need a built checkout (see the [README](https://github.com/nique-epi/R-Type#readme) and [CONTRIBUTING.md](https://github.com/nique-epi/R-Type/blob/main/CONTRIBUTING.md)) and a branch of your own started from `origin/main`. Check that the tests pass before you change anything, so a red test later is yours:

```bash
cmake --workflow --preset test
```

### What an enemy is here

A Bydo is an **entity** (a number) that carries **components** (small structs of data): where it is, how fast it goes, which behaviour pilots it, how many points it is worth. **Systems** (classes with an `update`) read and write components; they never know which enemy they act on. A type of entity is not C++ but a **sheet**, a TOML file of `assets/entities/` that lists the components an entity of that type starts with and how it looks (see [Writing an entity sheet](/R-Type/entity-sheets/)). So a new enemy is a new sheet, and sometimes a new **behaviour**, the named piece of code that decides where the Bydo goes.

The server owns the game. It creates the Bydos, moves them and tells each client about them, by the name of their sheet; the client draws each one with the appearance of the sheet of that name, its own or the one the content pack of the server carries (see [Architecture](/R-Type/architecture/#entity-types) and [Server and game](/R-Type/server/)).

| Step | What you add | Where |
|---|---|---|
| 1 | A new behaviour for the Bydo | `src/game/Behaviours/Movements/Paths/` |
| 2 | The sheet of the enemy | `assets/entities/zigzagger.toml` |
| 3 | Tests | `tests/` |
| 4 | The enemy in the waves | `assets/waves/waves.toml` |
| 5 | See it on screen | the client, unchanged |
| 6 | Document it | `docs/src/content/docs/` |

How a sheet reaches the world: each game process of the server reads every sheet of `assets/entities/` when it starts (`EntitySheets`), and the waves file names the enemies among them. `WaveSpawner` takes the names of the list in turn and calls `World::spawn(sheet, position)`, which gives the new entity its `Pilot` (the behaviour the sheet names, with the settings the sheet gives it), its `SteadyMotion`, its `Health`, its `ScoreValue` and its `Gun`. Each tick, `PilotSystem` lets the behaviour write the `Intent` of its Bydo, `MovementSystem` moves it by that `Intent`, and the weapon system fires it, exactly as it moves and fires a player: a behaviour that wants to shoot needs no other line of yours.

An enemy that an existing behaviour already moves needs no C++ at all: a Weaver twice as fast is a sheet with `pilot = "weave"` and `period_ms = 1000`. Skip step 1 then. The Zigzagger follows a path no behaviour draws yet, so it needs one.

An enemy that attacks, announcing a blow before it lands it, steps through the phases of its attack with `ActionCycle` (`src/game/Behaviours/Movements/Attacks/ActionCycle.hpp`), timed by settings of its sheet, and its sheet animates each phase and times the boxes that hurt (see [Writing an entity sheet](/R-Type/entity-sheets/#attack-animations)). `ReaperBehaviour.cpp` is the shortest example: a file of 74 lines for an enemy that hunts the nearest ship, raises its scythe, swings and stays open.

An enemy that sends several missiles or Bydos at once, as it enters an attack, needs no gun: each `[[gameplay.volley]]` of its sheet lays a fan, a column or a wall of bodies (see [Writing an entity sheet](/R-Type/entity-sheets/#gameplay-key-by-key)). A boss is an enemy whose sheet has a `[gameplay.boss]` table and that a difficulty names with its `boss` key (see [Writing the waves file](/R-Type/waves-file/#the-boss)); the Bydo Tyrant, `assets/entities/bydo_tyrant.toml` and `src/game/Behaviours/Movements/Attacks/BydoTyrantBehaviour.cpp`, is the example.

## Step 1: a new behaviour

A behaviour reads the Bydo it pilots (where it is, where it appeared, how old it is, the length of the tick, and the values its sheet gives the settings it declares) and returns what the Bydo wants at the end of the tick: a direction, a share of its top speed from -1 to 1 on each axis, and whether it fires (see [Architecture](/R-Type/architecture/#pilot-and-body)). Give yours a name, in `src/game/Behaviours/Movements/Paths/BehaviourConstants.hpp`:

```cpp
constexpr std::string_view ZIGZAG_BEHAVIOUR_NAME = "zigzag";
```

The zigzag reads the same two settings as the weave: `amplitude`, how far it goes above and below its row, in logical units, and `period_ms`, how long it takes to go down, up and back. So it declares the settings of the weave, `WEAVE_SETTINGS` in `WeaveBehaviourConstants.hpp`, and the reader of the sheets checks them the same way: an amplitude above `ENEMY_PATTERN_ROOM` (48), which would take the Bydo out of the area of play, stops the server when it reads the sheet. Its own numbers go in `src/game/Behaviours/Movements/Paths/ZigzagBehaviourConstants.hpp`, named and documented:

```cpp
#pragma once

namespace rtype::game {

/** @brief The shares of the period where a zigzag turns, and its slope. */
constexpr float QUARTER_CYCLE = 0.25F;
constexpr float THREE_QUARTERS_CYCLE = 0.75F;
constexpr float SLOPE_PER_CYCLE = 4.0F;

}  // namespace rtype::game
```

The behaviour is a class of `src/game/Behaviours/Movements/Paths/`, next to `WeaveBehaviour`; the contract it implements, `IBehaviour`, sits in `src/game/Behaviours/Piloting/`. Its header, `ZigzagBehaviour.hpp`:

```cpp
#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief Crosses the playfield towards the ships at its full speed, zigzagging
 * down and up around the row the body appeared on, and fires as often as its
 * Gun allows. It reads the settings of the weave (WEAVE_SETTINGS): the
 * `amplitude` of the zigzag, in logical units, and its `period_ms`.
 */
class ZigzagBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
```

Its source, `ZigzagBehaviour.cpp`, computes the height the zigzag reaches at the end of the tick from the age of the Bydo and its two settings, read with `settingAt()`, and asks `verticalShareTowards` (`BehaviourSteering.hpp`) for the share that lands there, as the weave does. The height is computed from the age, never added up tick by tick, so the path does not depend on how the time is cut into steps:

```cpp
#include "ZigzagBehaviour.hpp"
#include <chrono>
#include <cmath>
#include <span>
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "Intent.hpp"
#include "WeaveBehaviourConstants.hpp"
#include "ZigzagBehaviourConstants.hpp"

namespace rtype::game {

namespace {

/** @returns The height of a triangle wave at @p cycle, the share of the period
 * past, from -1 to 1: 0 at the start, 1 at a quarter, -1 at three quarters. */
float triangleWave(float cycle) {
  if (cycle < QUARTER_CYCLE) {
    return SLOPE_PER_CYCLE * cycle;
  }
  if (cycle < THREE_QUARTERS_CYCLE) {
    return 2.0F - (SLOPE_PER_CYCLE * cycle);
  }
  return (SLOPE_PER_CYCLE * cycle) - SLOPE_PER_CYCLE;
}

/** @returns Where the zigzag puts the center of the body of @p view at the end
 * of this tick, vertically, from the amplitude and the period of its
 * settings. */
float zigzagHeight(const BehaviourView& view) {
  const float ageSeconds = std::chrono::duration<float>(view.age).count();
  const auto amplitude = static_cast<float>(settingAt(view, AMPLITUDE_SETTING));
  const float periodSeconds =
      std::chrono::duration<float>(
          std::chrono::milliseconds{settingAt(view, PERIOD_SETTING)})
          .count();
  float wholeCycles = 0.0F;
  const float cycle = std::modf(ageSeconds / periodSeconds, &wholeCycles);
  return view.origin.y + (amplitude * triangleWave(cycle));
}

}  // namespace

std::string_view ZigzagBehaviour::name() const { return ZIGZAG_BEHAVIOUR_NAME; }

std::span<const BehaviourSetting> ZigzagBehaviour::settings() const {
  return WEAVE_SETTINGS;
}

Intent ZigzagBehaviour::steer(const BehaviourView& view) const {
  return Intent{.direction = Direction{.x = LEFTWARD_STEERING,
                                       .y = verticalShareTowards(
                                           zigzagHeight(view), view)},
                .shooting = true,
                .steered = true};
}

}  // namespace rtype::game
```

The height grows downwards on screen, so at a quarter of the period the Zigzagger is `amplitude` units below its row, as a Weaver is at a quarter of its own. A behaviour cannot move its Bydo faster than its top speed: with an amplitude of 48 and a period of 1000 ms, the zigzag climbs 4 × 48 = 192 units per second, under the 240 its sheet gives it in step 2, so it keeps its path.

Register it with the others in `builtInBehaviours()` (`src/game/Behaviours/Movements/BuiltInBehaviours.cpp`), and add `ZigzagBehaviour.cpp` to the sources of `src/game/Behaviours/Movements/Paths/CMakeLists.txt`:

```cpp
  behaviours.add(std::make_unique<ZigzagBehaviour>());
```

Build to check that it compiles with no warning:

```bash
cmake --build build --parallel
```

## Step 2: the sheet

The sheet is the whole enemy: its side, its pilot, its box, its speed, its health, its points, its gun and its look. Start from `assets/entities/weaver.toml`, which already holds a Bydo that fires, and save it as `assets/entities/zigzagger.toml`: the name of the file, `zigzagger`, is the name of the type. Change the pilot, the period and the points:

```toml
# The entity type zigzagger: a Bydo that zigzags down and up around its row.

[gameplay]
side = "enemy"
pilot = "zigzag"

[gameplay.hitbox]
width = 96
height = 96

[gameplay.motion]
kind = "steady"
top_speed = 240

[gameplay.behaviour]
amplitude = 48
period_ms = 1000

[gameplay.health]
points = 3

[gameplay.score]
points = 300

[gameplay.gun]
interval_ms = 1500
first_shot_ms = 1500
projectile = "enemy_missile"

[appearance]
sheet = "sprites/entities.png"

[appearance.idle]
left = 0
top = 72
width = 24
height = 24
frames = 2
spacing = 2
frame_ms = 250
```

Every key is described on [Writing an entity sheet](/R-Type/entity-sheets/). The server numbers the sheets in the order of their names, so `zigzagger` is type 6 and the five shipped sheets keep their numbers; nothing in the code or the protocol names it. The server and the client read the sheet when they start, so no rebuild is needed for this step once the behaviour is built.

The `[appearance]` above draws the Zigzagger as every other Bydo. To give it its own look, point `[appearance.idle]` at another row of the entity sheet: the bonus Bydo, right under the Bydo, `top = 98` with `frame_ms = 200`, makes it stand out with no change of the client or of the protocol. A new drawing goes into `assets/sprites/entities.png`, or into a sheet of its own named by `sheet`, and a frame keeps a quarter of the hitbox in pixels, 24 × 24 for 96 × 96.

A shipped sheet takes part in the fingerprint that tells vanilla games apart (see [Writing an entity sheet](/R-Type/entity-sheets/#the-shipped-sheets-and-vanilla-games)): from now on, `EntitySheetsFingerprint.TheShippedSheetsHaveTheOfficialFingerprint` fails until `OFFICIAL_ENTITY_SHEETS_FINGERPRINT` in `src/server/ServerConstants.hpp` takes the value its failure prints.

## Step 3: tests

Every test of this project is written as *Given / When / Then* in the comment above it, and must fail when the code is wrong. A test file holds only its tests: its constants go in the `…TestConstants.hpp` next to it. Add three tests, each next to the tests of the same part, and run them before and after to see them go from red to green.

The path, at the end of `tests/game/Systems/PilotSystemTest.cpp`, which pilots and moves a body as a game does. Its constants go in `PilotSystemTestConstants.hpp`, next to it, which then includes `BehaviourTestConstants.hpp` (where `WEAVER_AMPLITUDE_UNITS`, `ROW_Y` and `ROW_START` already sit, in `tests/game/Behaviours/`), `TimeConstants.hpp`, `<array>`, `<chrono>` and `<cstdint>`; each new name gets its `using` line at the top of the test file, as the names around it do:

```cpp
constexpr std::int64_t ZIGZAGGER_PERIOD_MS = 1000;
constexpr std::array ZIGZAGGER_SETTINGS{WEAVER_AMPLITUDE_UNITS,
                                        ZIGZAGGER_PERIOD_MS};
constexpr engine::Duration QUARTER_ZIGZAG_PERIOD =
    std::chrono::milliseconds(250);
constexpr engine::Duration HALF_ZIGZAG_PERIOD = std::chrono::milliseconds(500);
constexpr float ZIGZAG_HEIGHT = 48.0F;
```

```cpp
/**
 * Given a zigzagging body on the row at height 300, 48 units deep every second
 * When a quarter, then three quarters, then a whole period has passed
 * Then it is 48 units below its row, then 48 units above it, then back on it
 */
TEST_F(PilotSystemFixture, AZigzagGoesDownThenUpThenBackToItsRow) {
  const Entity body =
      addBody(ZIGZAG_BEHAVIOUR_NAME, ZIGZAGGER_SETTINGS, ROW_START);

  run(QUARTER_ZIGZAG_PERIOD);
  EXPECT_NEAR(positionOf(body).y, ROW_Y + ZIGZAG_HEIGHT,
              HEIGHT_TOLERANCE_UNITS);

  run(HALF_ZIGZAG_PERIOD);
  EXPECT_NEAR(positionOf(body).y, ROW_Y - ZIGZAG_HEIGHT,
              HEIGHT_TOLERANCE_UNITS);

  run(QUARTER_ZIGZAG_PERIOD);
  EXPECT_NEAR(positionOf(body).y, ROW_Y, HEIGHT_TOLERANCE_UNITS);
}
```

The sheet, its behaviour and its points, at the end of `tests/WorldTest.cpp`, with the shipped sheets `shippedEntitySheets()` reads from `assets/entities/`. Its constants go in `tests/game/EntityTypes/EntityTypesTestConstants.hpp`, next to `WEAVER_SHEET` and `REQUIRED_WEAVER_POINTS`:

```cpp
constexpr std::string_view ZIGZAGGER_SHEET = "zigzagger";
constexpr std::uint32_t REQUIRED_ZIGZAGGER_POINTS = 300;
```

```cpp
/**
 * Given a world
 * When a Zigzagger is spawned from its sheet
 * Then it is piloted by the zigzag and is worth the points of a Zigzagger
 */
TEST(World, ASpawnedZigzaggerZigzagsAndIsWorthItsPoints) {
  rtype::game::World world;

  const std::uint32_t zigzagger =
      world.spawn(shippedEntitySheets().named(ZIGZAGGER_SHEET),
                  rtype::game::Position{.x = SOME_X, .y = SOME_Y});

  EXPECT_EQ(behaviourNameOf(world, zigzagger).value_or(""),
            ZIGZAG_BEHAVIOUR_NAME);
  EXPECT_EQ(componentOf<rtype::game::ScoreValue>(world, zigzagger)
                .value_or(rtype::game::ScoreValue{})
                .points,
            REQUIRED_ZIGZAGGER_POINTS);
}
```

The name in the waves file, at the end of `tests/server/Spawning/WaveCatalogTest.cpp`:

```cpp
/**
 * Given a waves file whose calm difficulty lists the enemy zigzagger
 * When it is read against the shipped sheets
 * Then the enemies of calm are one Zigzagger
 */
TEST(WaveCatalog, ReadsTheZigzaggerByItsName) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFileWith("enemies", R"(["zigzagger"])"),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").enemies,
            std::vector<std::string>{std::string{ZIGZAGGER_SHEET}});
}
```

Two tests that already exist change too, and the suite shows them: the fingerprint of step 2, and `WaveCatalog.AnUnknownEnemyIsNamedWithTheKnownOnes`, which pins the message the server prints for a misspelt enemy, listing the enemy sheets it knows in the order of their names. Add yours to `UNKNOWN_ENEMY_MESSAGE` in `tests/server/Spawning/WaveCatalogTestConstants.hpp`:

```cpp
constexpr std::string_view UNKNOWN_ENEMY_MESSAGE =
    R"(test.toml:8:23: difficulty.calm.enemies: unknown enemy "bos"; known: drifter, weaver, zigzagger)";
```

Run the whole suite; it must pass with no warning:

```bash
cmake --workflow --preset test
```

## Step 4: put the enemy in the waves

Open `assets/waves/waves.toml`. The `enemies` list of a difficulty is the order its Bydos take, one after the other, from one wave to the next, each name the name of an enemy sheet. To meet the Zigzagger at once, make it the only enemy of `normal`, the difficulty every game plays:

```toml
enemies = ["zigzagger"]
```

Mixing sheets works the same way: `["drifter", "weaver", "zigzagger"]` brings one of each in turn, and a name written twice weights its sheet. The file is read when the server starts, so no rebuild is needed for this step (see [Writing the waves file](/R-Type/waves-file/)). A change of the shipped waves file makes `WaveCatalog.TheShippedFileHasTheOfficialFingerprint` fail, as the sheet did in step 2: put the file back before you commit, or update `OFFICIAL_WAVES_FINGERPRINT` with it.

## Step 5: see it on screen

Build the game with its binaries at the repository root, start the server, then a client in another terminal:

```bash
cmake --workflow --preset build
./r-type_server --seed 42
```

```bash
./r-type_client
```

In the client, press PLAY, then D (ADDRESS) on the server map, then CONNECT on the connection screen (the server is at `127.0.0.1`, port 4242, by default), press ready in the waiting room, and wait for the first wave. The supervisor starts a game process for you, which logs in its journal, `r-type_game_<n>.log` next to the executable, the sheets it read when it starts (`entity sheets /…/assets/entities: 16 types`). Each Bydo goes left while it dips and rises every second, 48 units each way, and shooting one adds 300 points to your score. The same journal gives the wave as it starts (`wave 1: 3 Bydos, one every 2000 ms`).

What to look at, and why: **the client needed no change**. When it accepts the player, the server announces the content pack of its supervisor: every sheet of `assets/entities/`, `zigzagger.toml` included, and the sprite sheets they name, then names each entity by the number of its sheet. This client read the same `assets/` folder, so its own pack has the same fingerprint and it plays at once, drawing each Zigzagger with the `[appearance]` of its own `zigzagger.toml`; the behaviour and the rest of the gameplay never leave the server. A client whose `assets/` differ, an older build for instance, downloads the pack of the server while its connection screen shows `LOADING CONTENT` and a percentage, then draws the Zigzagger from the sheet of the pack (see [Tutorial: publish a mod](/R-Type/tutorial-publish-a-mod/)).

## Step 6: document it

A feature is finished when another person can find it. Add your enemy to the **Enemies** table of `docs/src/content/docs/waves-file.md` (name, sheet, how it moves, points) and to the table of the shipped sheets of `docs/src/content/docs/entity-sheets.md`, fix the `known: ...` lists of the error examples of both pages, and, for a new behaviour, add its row and its settings to the table of the behaviours of `entity-sheets.md`. Then build the site, which checks every link:

```bash
cd docs && npm ci && npm run build
```

## Checklist

- [ ] `cmake --workflow --preset test` passes with no warning.
- [ ] `cmake --build build --target format-check` passes (run `--target format` to fix it).
- [ ] `cmake --build build --target tidy` passes.
- [ ] The server starts with the sheet and the waves file, and the enemy appears.
- [ ] The doc pages name the new enemy.
- [ ] One commit per intent, in English, with a Conventional Commits header (see [CONTRIBUTING.md](https://github.com/nique-epi/R-Type/blob/main/CONTRIBUTING.md)): `feat: add the Zigzagger, a Bydo that zigzags`.

## When something goes wrong

| What you see | What it means |
|---|---|
| The server stops with `gameplay.pilot: unknown value "zigzag"; known: player, straight, weave` | the behaviour is not added in `builtInBehaviours()`, or the server was not rebuilt after step 1 |
| The server stops with `unknown enemy "zigzagger"; known: drifter, weaver` | `assets/entities/zigzagger.toml` is missing, misspelt, or not of side `enemy` |
| The server stops with `gameplay.behaviour.amplitude must be an integer from 0 to 48` | the amplitude is larger than `ENEMY_PATTERN_ROOM`: it would leave the area of play |
| The server or the client stops at start with an error naming a line of `zigzagger.toml` | the sheet is invalid; the message names the line and the key (see [Writing an entity sheet](/R-Type/entity-sheets/#when-a-program-refuses-a-sheet)) |
| `EntitySheetsFingerprint.TheShippedSheetsHaveTheOfficialFingerprint` fails | the shipped sheets changed: give `OFFICIAL_ENTITY_SHEETS_FINGERPRINT` the value the failure prints |
| The connection screen shows `PACK DOWNLOAD FAILED` | the client reads an `assets/` folder other than the server's, and cannot download the pack: the TCP port of the supervisor, of the same number as its UDP port, is closed or held by another program (the journal of the supervisor says `the content pack is not served`) |
| The supervisor stops at start with `Refused content pack: "…" may not name a file` | the `sheet` of `zigzagger.toml` names a path with an uppercase letter, a space or `..`: a path of a content pack holds lowercase letters, digits, `_`, `.`, `-` and `/` only |
| The Bydos move but do not zigzag | the enemy is not in the `enemies` list of the difficulty you play (`normal`) |
