---
title: "Tutorial: add an enemy type"
description: A step-by-step walk from the movement of a new Bydo to its appearance on screen, with the code to write, the tests to add and the commands to check each step.
sidebar:
  label: "Tutorial: an enemy type"
---

This tutorial adds an enemy to the game, from the component that moves it to the screen that shows it. It is written for someone who has not touched the C++ of this project yet: every step names the file, gives the code and ends with a command that tells you it worked. Nothing here needs the help of the team.

The enemy is the **Zigzagger**: a Bydo that goes down, up and back to its row twice as fast as a [Weaver](/R-Type/waves-file/#enemies) weaves, and is worth 300 points. It is only an example: at the end you will have the steps to add your own, and you can keep your own enemy instead.

## Before you start

You need a built checkout (see the [README](https://github.com/nique-epi/R-Type#readme) and [CONTRIBUTING.md](https://github.com/nique-epi/R-Type/blob/main/CONTRIBUTING.md)) and a branch of your own started from `origin/main`. Check that the tests pass before you change anything, so a red test later is yours:

```bash
cmake --workflow --preset test
```

### What an enemy is here

A Bydo is an **entity** (a number) that carries **components** (small structs of data): where it is, how fast it goes, what path it follows, how many points it is worth. **Systems** (classes with an `update`) read and write components; they never know which enemy they act on. So a new enemy is rarely new code for the systems: it is a new combination of components, and sometimes a new path for the system that moves it.

The server owns the game. It creates the Bydos, moves them and tells each client about them; the client only draws what it is told (see [Architecture](/R-Type/architecture/) and [Server and game](/R-Type/server/)).

| Step | What you add | Where |
|---|---|---|
| 1 | A new path for the Bydo to follow | `src/game/Components/MovementPattern.hpp`, `src/game/Systems/PatternSystem/` |
| 2 | The kind of enemy and its points | `src/game/Spawning/BydoKind/` |
| 3 | The name the waves file gives it | `src/server/Spawning/WaveFileConstants.hpp` |
| 4 | Tests | `tests/` |
| 5 | The enemy in the waves | `assets/waves/waves.toml` |
| 6 | See it on screen | the client, unchanged |
| 7 | Document it | `docs/src/content/docs/` |

How a kind reaches the world: `BydoKind` is read by `World::spawnBydo`, which gives the new entity its `MovementPattern` (the path, from `patternShapeOf(kind)`) and its `ScoreValue` (from `pointsOf(kind)`). `BydoSpawner` picks the kinds in turn from the waves file. `BydoControlSystem` makes every Bydo shoot, and the weapon system fires it, exactly as it fires a player: a new kind shoots without a line of yours.

## Step 1: a new path

The path of a Bydo is a `PatternShape`. Add yours to the enum, in `src/game/Components/MovementPattern.hpp`:

```cpp
enum class PatternShape : std::uint8_t {
  Straight,
  Wave,
  Zigzag,
};
```

Its numbers go in `src/game/Systems/PatternSystem/PatternConstants.hpp`, named and documented, above the `static_assert` of the wave. The `static_assert` is the guard: a Bydo must stay inside the room its row keeps above and below it (`BYDO_PATTERN_ROOM`), or it leaves the area of play.

```cpp
/**
 * @brief How far a Zigzag goes above and below its row, in logical units.
 *
 * Provisional. It stays within BYDO_PATTERN_ROOM, like WAVE_AMPLITUDE.
 */
constexpr float ZIGZAG_AMPLITUDE = 48.0F;

/**
 * @brief How long a Zigzag takes to go down, up and back to its row, in
 * seconds.
 */
constexpr float ZIGZAG_PERIOD_SECONDS = 1.0F;

static_assert(ZIGZAG_AMPLITUDE <= BYDO_PATTERN_ROOM,
              "a Bydo on a zigzag stays within the room its row keeps");
```

`PatternSystem` turns the shape into a height. In `src/game/Systems/PatternSystem/PatternSystem.cpp`, add the triangle wave next to `heightOffset`, and a case for your shape inside it (the `switch` has no `default`, so the compiler warns about the shape you forgot, and warnings are errors here):

```cpp
constexpr float QUARTER_CYCLE = 0.25F;
constexpr float THREE_QUARTERS_CYCLE = 0.75F;
constexpr float SLOPE_PER_CYCLE = 4.0F;

/** @returns The height of a triangle wave at @p cycle, the share of the period
 * past, from -1 to 1: 0 at the start, 1 at a quarter, -1 at three quarters. */
float triangleWave(float cycle) {
  if (cycle < QUARTER_CYCLE) {
    return SLOPE_PER_CYCLE * cycle;
  }
  if (cycle < THREE_QUARTERS_CYCLE) {
    return 2.0F - SLOPE_PER_CYCLE * cycle;
  }
  return SLOPE_PER_CYCLE * cycle - SLOPE_PER_CYCLE;
}
```

```cpp
    case PatternShape::Zigzag: {
      double wholeCycles = 0.0;
      const float cycle = static_cast<float>(
          std::modf(ageSeconds / ZIGZAG_PERIOD_SECONDS, &wholeCycles));
      return ZIGZAG_AMPLITUDE * triangleWave(cycle);
    }
```

The height grows downwards on screen, so at a quarter of the period the Zigzagger is 48 units below its row, as a Weaver is at a quarter of its own. Build to check that it compiles with no warning:

```bash
cmake --build build --parallel
```

## Step 2: the kind and its points

A `BydoKind` is the name of an enemy in the code. In `src/game/Spawning/BydoKind/BydoKind.hpp`, add the enumerator (and say what it does in the doc comment of `patternShapeOf`):

```cpp
enum class BydoKind : std::uint8_t {
  Drifter,
  Weaver,
  Zigzagger,
};
```

Its points are a named constant in `src/game/Spawning/BydoKind/BydoPointsConstants.hpp`:

```cpp
/**
 * @brief Points earned for destroying a Zigzagger, which changes direction
 * twice as fast as a Weaver.
 *
 * Provisional: gameplay will tune it.
 */
constexpr std::uint32_t ZIGZAGGER_POINTS = 300;
```

And the two functions of `src/game/Spawning/BydoKind/BydoKind.cpp` each get a case, which is the whole link between your kind, its path and its score:

```cpp
    case BydoKind::Zigzagger:
      return PatternShape::Zigzag;
```

```cpp
    case BydoKind::Zigzagger:
      return ZIGZAGGER_POINTS;
```

Nothing else in the game needs to know the kind: `World::spawnBydo` calls these two functions and gives the Bydo the same health, size, speed and gun as the others. To give your enemy more health, a different speed or another shot interval, that is where it differs: read `World::spawnBydo` in `src/game/World.cpp` and pass the kind to the constant you change.

## Step 3: the name in the waves file

The waves file names enemies with a word. In `src/server/Spawning/WaveFileConstants.hpp`, give your kind its name and list it in `ENEMY_NAMES`:

```cpp
constexpr std::string_view ZIGZAGGER_NAME = "zigzagger";
constexpr std::array ENEMY_NAMES{
    std::pair{DRIFTER_NAME, game::BydoKind::Drifter},
    std::pair{WEAVER_NAME, game::BydoKind::Weaver},
    std::pair{ZIGZAGGER_NAME, game::BydoKind::Zigzagger}};
```

A file that names an enemy missing from this list stops the server with an error that lists the names it knows, so a typo never goes unnoticed.

## Step 4: tests

Every test of this project is written as *Given / When / Then* in the comment above it, and must fail when the code is wrong. Add three, each next to the tests of the same part, and run them before and after to see them go from red to green.

The path, at the end of `tests/PatternSystemTest.cpp` (the constants go in its anonymous namespace, with the others):

```cpp
constexpr Duration QUARTER_ZIGZAG_PERIOD = std::chrono::milliseconds(250);
constexpr Duration HALF_ZIGZAG_PERIOD = std::chrono::milliseconds(500);
constexpr float ZIGZAG_AMPLITUDE = 48.0F;
```

```cpp
/**
 * Given an entity on a Zigzag pattern, on the row at height 300
 * When a quarter, then three quarters, then a whole 1 second period has passed
 * Then it is 48 units below its row, then 48 units above it, then back on it
 */
TEST(PatternSystem, ZigzagGoesDownThenUpThenBackToItsRow) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity entity =
      addPatterned(entities, components, PatternShape::Zigzag);
  PatternSystem pattern;

  pattern.update(components, QUARTER_ZIGZAG_PERIOD);
  EXPECT_NEAR(components.get<Position>(entity)->y, ROW_Y + ZIGZAG_AMPLITUDE,
              HEIGHT_TOLERANCE_UNITS);

  pattern.update(components, HALF_ZIGZAG_PERIOD);
  EXPECT_NEAR(components.get<Position>(entity)->y, ROW_Y - ZIGZAG_AMPLITUDE,
              HEIGHT_TOLERANCE_UNITS);

  pattern.update(components, QUARTER_ZIGZAG_PERIOD);
  EXPECT_NEAR(components.get<Position>(entity)->y, ROW_Y,
              HEIGHT_TOLERANCE_UNITS);
}
```

The kind, its path and its points, at the end of `tests/WorldTest.cpp`:

```cpp
/**
 * Given a world
 * When a Zigzagger is spawned
 * Then it follows a zigzag and is worth the points of a Zigzagger
 */
TEST(World, ASpawnedZigzaggerFollowsAZigzagAndIsWorthItsPoints) {
  rtype::game::World world;

  static_cast<void>(
      world.spawnBydo(rtype::game::Position{.x = SOME_X, .y = SOME_Y},
                      rtype::game::BydoKind::Zigzagger));

  std::size_t zigzags = 0;
  world.components()
      .forEach<rtype::game::MovementPattern, rtype::game::ScoreValue>(
          [&zigzags](rtype::engine::Entity,
                     const rtype::game::MovementPattern& pattern,
                     const rtype::game::ScoreValue& score) {
            if (pattern.shape == rtype::game::PatternShape::Zigzag &&
                score.points == rtype::game::ZIGZAGGER_POINTS) {
              ++zigzags;
            }
          });
  EXPECT_EQ(zigzags, 1U);
}
```

The name in the waves file, at the end of `tests/server/Spawning/WaveCatalogTest.cpp`:

```cpp
/**
 * Given a waves file whose calm difficulty lists the enemy zigzagger
 * When it is read
 * Then the enemies of calm are one Zigzagger
 */
TEST(WaveCatalog, ReadsTheZigzaggerByItsName) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      calmFileWith("enemies", R"(["zigzagger"])"), TEST_FILE_NAME);

  EXPECT_EQ(catalog.planOf("calm").enemies,
            std::vector<rtype::game::BydoKind>{
                rtype::game::BydoKind::Zigzagger});
}
```

One test that already exists changes too, and the suite shows it: `WaveCatalog.AnUnknownEnemyIsNamedWithTheKnownOnes` pins the message the server prints for a misspelt enemy, which lists the names it knows. Add yours to `UNKNOWN_ENEMY_MESSAGE` in `tests/server/Spawning/WaveCatalogTestConstants.hpp`, in the order of `ENEMY_NAMES`:

```cpp
constexpr std::string_view UNKNOWN_ENEMY_MESSAGE =
    R"(test.toml:8:23: difficulty.calm.enemies: unknown enemy "bos"; known: drifter, weaver, zigzagger)";
```

Run the whole suite; it must pass with no warning:

```bash
cmake --workflow --preset test
```

## Step 5: put the enemy in the waves

Open `assets/waves/waves.toml`. The `enemies` list of a difficulty is the order its Bydos take, one after the other, from one wave to the next. To meet the Zigzagger at once, make it the only enemy of `normal`, the difficulty every game plays:

```toml
enemies = ["zigzagger"]
```

Mixing kinds works the same way: `["drifter", "weaver", "zigzagger"]` brings one of each in turn, and a name written twice weights its kind. The file is read when the server starts, so no rebuild is needed for this step (see [Writing the waves file](/R-Type/waves-file/)).

## Step 6: see it on screen

Build the game with its binaries at the repository root, start the server, then a client in another terminal:

```bash
cmake --workflow --preset build
./r-type_server --seed 42
```

```bash
./r-type_client
```

On the connection screen of the client, press CONNECT (the server is at `127.0.0.1`, port 4242, by default), press ready in the waiting room, and wait for the first wave. Each Bydo goes left while it dips and rises every second, 48 units each way, and shooting one adds 300 points to your score. The server prints the wave as it starts (`wave 1: 3 Bydos, one every 2000 ms`).

What to look at, and why: **the Zigzagger is drawn as every other Bydo**. The server tells the client "a Bydo appeared", then sends the position of each Bydo twenty times a second; the kind never leaves the server, which is why the client needed no change to show the new path. The client chooses a sprite from the type of the entity (`entityAnimationOf` in `src/client/Rendering/Sprites/EntitySprites/EntitySprites.cpp`), and every Bydo is an `EntityType::Enemy`.

### Giving the enemy its own look

A different sprite is a change of the protocol, not of the client alone, because the client only learns the type of an entity: the kind would have to travel too. Read it as a list of places to open, each found by following `Bydo` through the code, not as steps this tutorial ran:

- the entity type of the protocol, `WireEntityType` and its list of valid values in `src/network/Protocol/Messages/EntitySpawned/EntitySpawned.cpp`, and the [Protocol](/R-Type/protocol/) page that lists it;
- the two conversions, `toWireEntityType` on the server (`src/server/Replication/WireEntityTypeMapping/`) and `entityTypeOf` on the client (`src/client/Replication/EntityReplicator/`);
- the sprite of the client, its frames on `sprites/entities.png` and the table of `assets/README.md`.

Talk to the team before changing the protocol: a client and a server of different versions must keep understanding each other.

## Step 7: document it

A feature is finished when another person can find it. Add your enemy to the **Enemies** table of `docs/src/content/docs/waves-file.md` (name, kind, how it moves, points), fix the `known: ...` list of the error example on the same page, and update the line of `docs/src/content/docs/server.md` that says which shape each kind follows. Then build the site, which checks every link:

```bash
cd docs && npm ci && npm run build
```

## Checklist

- [ ] `cmake --workflow --preset test` passes with no warning.
- [ ] `cmake --build build --target format-check` passes (run `--target format` to fix it).
- [ ] `cmake --build build --target tidy` passes.
- [ ] The waves file starts the server and the enemy appears.
- [ ] The doc pages name the new enemy.
- [ ] One commit per intent, in English, with a Conventional Commits header (see [CONTRIBUTING.md](https://github.com/nique-epi/R-Type/blob/main/CONTRIBUTING.md)): `feat: add the Zigzagger, a Bydo that zigzags`.

## When something goes wrong

| What you see | What it means |
|---|---|
| `enumeration value 'Zigzag' not handled in switch` | a `switch` over `PatternShape` or `BydoKind` has no case for yours; add it where the compiler points |
| The server stops with `unknown enemy "..."; known: ...` | the name in `waves.toml` is not in `ENEMY_NAMES` (step 3), or is misspelt |
| The server stops at start with an error naming a line of `waves.toml` | the file is invalid; the message names the line and the key |
| The Bydos move but do not zigzag | the enemy is not in the `enemies` list of the difficulty you play (`normal`) |
| `static assertion failed: a Bydo on a zigzag stays within the room its row keeps` | the amplitude is larger than `BYDO_PATTERN_ROOM`: it would leave the area of play |
