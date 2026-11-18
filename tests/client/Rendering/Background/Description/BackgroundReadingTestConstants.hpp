#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief The name the tests give their background and its file, which the
 * errors show. */
constexpr std::string_view READ_BACKGROUND_NAME = "space";
constexpr std::string_view READ_BACKGROUND_FILE = "space.toml";

/** @brief What particlesReading() writes before the keys of its particles:
 * the group the particles of a group name, then a layer that holds them. */
constexpr std::string_view PARTICLE_LAYER_START = R"(
[[group]]
name = "sky events"
most_alive = 4
first_ms = 600
every_ms = [800, 2600]

[[layer]]
name = "particles"

[layer.particles]
)";

/** @brief What layerReading() writes before the keys of its layer. */
constexpr std::string_view LAYER_START = "[[layer]]\nname = \"layer\"\n";

/** @brief A planet that moves, then the walls of the Pit, which stay and are
 * the entrance. */
constexpr std::string_view A_PLANET_THEN_WALLS = R"(
[[layer]]
name = "planet"
motion = { parallax = 0.5, autoscroll = 1.8, repeat = 647, warps = true }
image = { file = "backgrounds/space/planet.png", x = 288, y = 43 }

[[layer]]
name = "near walls"
entrance = true
tiles = { project = "biomes/fosse/fosse.ldtk", layer = "Near", tileset = "biomes/fosse/fosse_near.png" }
)";
constexpr std::size_t TWO_LAYERS = 2;
constexpr std::size_t PLANET_LAYER = 0;
constexpr std::size_t WALL_LAYER = 1;
constexpr std::string_view PLANET_NAME = "planet";
constexpr std::string_view WALL_NAME = "near walls";
constexpr float HALF_THE_SCROLL = 0.5F;
constexpr float PLANET_AUTOSCROLL = 1.8F;
constexpr float PLANET_REPEAT = 647.0F;
constexpr std::string_view PLANET_FILE = "backgrounds/space/planet.png";
constexpr sf::Vector2f PLANET_TOP_LEFT(288.0F, 43.0F);
constexpr std::string_view FOSSE_PROJECT = "biomes/fosse/fosse.ldtk";
constexpr std::string_view NEAR_LAYER_OF_THE_PROJECT = "Near";
constexpr std::string_view NEAR_TILESET = "biomes/fosse/fosse_near.png";

/** @brief The content of a layer, as layerReading() takes it. */
constexpr std::string_view AN_IMAGE_WITHOUT_A_PLACE =
    "image = { file = \"backgrounds/space/planet.png\" }\n";
constexpr std::string_view CRATES =
    "props = { project = \"biomes/fosse/fosse.ldtk\", layer = \"Props\", "
    "tileset = \"biomes/fosse/props_16.png\" }\n";
constexpr std::string_view PROP_LAYER_OF_THE_PROJECT = "Props";
constexpr std::string_view PROP_TILESET = "biomes/fosse/props_16.png";

/** @brief The colors the names of the palette stand for. */
constexpr sf::Color DEEP_BLUE_OF_THE_PALETTE(38, 48, 94);
constexpr sf::Color BLUE_OF_THE_PALETTE(74, 90, 168);
constexpr sf::Color PALE_BLUE_OF_THE_PALETTE(143, 162, 230);
constexpr sf::Color WHITE_OF_THE_PALETTE(238, 241, 255);

/** @brief The far stars: a population of pixels that twinkle. */
constexpr std::string_view FAR_STARS = R"(
population = 441
area = { left = 0, top = 0, width = 480, height = 270 }
pixel = "deep_blue"
twinkle = "blue"
twinkle_steps = 8
warp_trail = "deep_blue"
)";
constexpr std::size_t FAR_STAR_COUNT = 441;
constexpr sf::FloatRect WHOLE_BACKGROUND({0.0F, 0.0F}, {480.0F, 270.0F});
constexpr std::int64_t ONE_STEP_IN_EIGHT = 8;

/** @brief A population of pixels that says nothing else. */
constexpr std::string_view PLAIN_STARS = R"(
population = 90
area = { left = 0, top = 0, width = 480, height = 270 }
pixel = "pale_blue"
)";

/** @brief The near stars: a population drawn with two sprites, the first
 * four times as likely as the second. */
constexpr std::string_view NEAR_STARS = R"(
population = 36
area = { left = 0, top = 0, width = 480, height = 270 }
twinkle_steps = 8
sprites = [
  { file = "backgrounds/space/near_star.png", frames = 2, weight = 4, anchor_x = 1, anchor_y = 1 },
  { file = "backgrounds/space/warm_star.png" },
]
)";
constexpr std::size_t TWO_SPRITES = 2;
constexpr std::string_view NEAR_STAR_FILE = "backgrounds/space/near_star.png";
constexpr std::size_t TWO_FRAMES = 2;
constexpr double FOUR_TIMES_AS_LIKELY = 4.0;
constexpr sf::Vector2f MIDDLE_OF_A_NEAR_STAR(1.0F, 1.0F);
constexpr std::size_t ONE_FRAME = 1;
constexpr double AS_LIKELY_AS_ANY = 1.0;
constexpr sf::Vector2f TOP_LEFT_OF_A_FRAME(0.0F, 0.0F);

/** @brief The asteroids: particles of a group that give every key of their
 * flight, drawn with a sprite that spins. */
constexpr std::string_view ASTEROIDS = R"(
group = "sky events"
weight = 1.5
area = { left = 486, top = 0, width = 0, height = 270 }
speed = [42, 108]
angle = [170, 190]
drift = 9
life_ms = [2000, 3000.5]
margin = 6
sprites = [
  { file = "backgrounds/space/asteroid.png", frames = 4, fps = [6, 18] },
]
)";
constexpr std::string_view SKY_EVENT_GROUP = "sky events";
constexpr double HALF_AS_LIKELY_AGAIN = 1.5;
constexpr sf::FloatRect RIGHT_OF_THE_BACKGROUND({486.0F, 0.0F}, {0.0F, 270.0F});
constexpr double SLOWEST_ASTEROID = 42.0;
constexpr double FASTEST_ASTEROID = 108.0;
constexpr double HIGHEST_ASTEROID_ANGLE = 170.0;
constexpr double LOWEST_ASTEROID_ANGLE = 190.0;
constexpr float ASTEROID_DRIFT = 9.0F;
constexpr std::chrono::microseconds SHORTEST_ASTEROID_LIFE{2'000'000};
constexpr std::chrono::microseconds LONGEST_ASTEROID_LIFE{3'000'500};
constexpr float ASTEROID_MARGIN = 6.0F;
constexpr double SLOWEST_SPIN = 6.0;
constexpr double FASTEST_SPIN = 18.0;

/** @brief Particles of a group that give only what they must: their group,
 * where they are born, their look and a speed. */
constexpr std::string_view PLAIN_ASTEROIDS = R"(
group = "sky events"
area = { left = 486, top = 0, width = 0, height = 270 }
speed = [42, 108]
pixel = "star_white"
)";
constexpr double TO_THE_LEFT = 180.0;

/** @brief The shooting stars: particles of a group with a trail of two
 * colors. */
constexpr std::string_view SHOOTING_STARS = R"(
group = "sky events"
area = { left = 168, top = 0, width = 312, height = 121.5 }
speed = [300, 510]
life_ms = [133, 333]
pixel = "star_white"

[layer.particles.trail]
longest = 12
grow = 180
shrink = 150
stops = [
  { until = 0.5, color = "star_white" },
  { until = 1, color = "deep_blue" },
]
)";
constexpr float LONGEST_TRAIL = 12.0F;
constexpr float TRAIL_GROWTH = 180.0F;
constexpr float TRAIL_SHRINK = 150.0F;
constexpr std::size_t TWO_STOPS = 2;
constexpr float HALF_THE_TRAIL = 0.5F;
constexpr float THE_WHOLE_TRAIL = 1.0F;

/** @brief What the group of PARTICLE_LAYER_START says. */
constexpr std::size_t ONE_GROUP = 1;
constexpr std::size_t FOUR_AT_ONCE = 4;
constexpr std::chrono::milliseconds FIRST_BIRTH{600};
constexpr std::chrono::milliseconds SHORTEST_BIRTH_INTERVAL{800};
constexpr std::chrono::milliseconds LONGEST_BIRTH_INTERVAL{2600};

/** @brief Files that are refused as a whole. */
constexpr std::string_view NOT_TOML = "[[layer]]\nname =\n";
constexpr std::string_view NO_LAYER =
    "[[group]]\nname = \"sky events\"\nmost_alive = 4\nfirst_ms = 600\n"
    "every_ms = [800, 2600]\n";
constexpr std::string_view MISSING_LAYERS_TEXT = "layer is missing";

/** @brief Files that hold one key their table does not know, `sidd`, each in
 * another table. */
constexpr std::string_view UNKNOWN_KEY_OF_THE_ROOT =
    "sidd = 1\n[[layer]]\nname = \"layer\"\nimage = { file = \"a.png\" }\n";
constexpr std::string_view UNKNOWN_KEY_OF_A_LAYER =
    "[[layer]]\nname = \"layer\"\nsidd = 1\nimage = { file = \"a.png\" }\n";
constexpr std::string_view UNKNOWN_KEY_OF_A_MOTION =
    "[[layer]]\nname = \"layer\"\nmotion = { sidd = 1 }\n"
    "image = { file = \"a.png\" }\n";
constexpr std::string_view UNKNOWN_KEY_OF_AN_IMAGE =
    "[[layer]]\nname = \"layer\"\nimage = { file = \"a.png\", sidd = 1 }\n";
constexpr std::string_view UNKNOWN_KEY_OF_TILES =
    "[[layer]]\nname = \"layer\"\ntiles = { project = \"a.ldtk\", "
    "layer = \"Near\", tileset = \"a.png\", sidd = 1 }\n";
constexpr std::string_view UNKNOWN_KEY_OF_A_GROUP =
    "[[group]]\nname = \"sky events\"\nmost_alive = 4\nfirst_ms = 600\n"
    "every_ms = [800, 2600]\nsidd = 1\n[[layer]]\nname = \"layer\"\n"
    "image = { file = \"a.png\" }\n";

/** @brief Particles that hold one key they do not know: `sidd`, in the
 * particles, their area, a sprite, their trail or one of its stops; and a key
 * of the particles of a group in a population, and the other way round. */
constexpr std::string_view UNKNOWN_KEY_OF_A_POPULATION =
    "population = 1\narea = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nsidd = 1\n";
constexpr std::string_view UNKNOWN_KEY_OF_AN_AREA =
    "population = 1\n"
    "area = { left = 0, top = 0, width = 1, height = 1, sidd = 1 }\n"
    "pixel = \"blue\"\n";
constexpr std::string_view UNKNOWN_KEY_OF_A_SPRITE =
    "population = 1\narea = { left = 0, top = 0, width = 1, height = 1 }\n"
    "sprites = [{ file = \"a.png\", sidd = 1 }]\n";
constexpr std::string_view A_SPEED_IN_A_POPULATION =
    "population = 1\narea = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nspeed = [1, 2]\n";
constexpr std::string_view A_WARP_TRAIL_IN_A_GROUP =
    "group = \"sky events\"\n"
    "area = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nspeed = [1, 2]\nwarp_trail = \"blue\"\n";
constexpr std::string_view UNKNOWN_KEY_OF_A_TRAIL =
    "group = \"sky events\"\n"
    "area = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nspeed = [1, 2]\n"
    "trail = { longest = 12, grow = 180, shrink = 150, stops = [], "
    "sidd = 1 }\n";
constexpr std::string_view UNKNOWN_KEY_OF_A_TRAIL_STOP =
    "group = \"sky events\"\n"
    "area = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nspeed = [1, 2]\n"
    "trail = { longest = 12, grow = 180, shrink = 150, "
    "stops = [{ until = 1, color = \"blue\", sidd = 1 }] }\n";

/** @brief Layers whose keys do not go together, as layerReading() takes
 * them, and what the error says of each. */
constexpr std::string_view NO_CONTENT = "motion = { parallax = 1 }\n";
constexpr std::string_view AN_IMAGE_AND_TILES =
    "image = { file = \"a.png\" }\ntiles = { project = \"a.ldtk\", "
    "layer = \"Near\", tileset = \"a.png\" }\n";
constexpr std::string_view ONE_CONTENT_REFUSAL =
    "layer[0]: a layer holds exactly one of image, tiles, particles and props";
constexpr std::string_view TWO_ENTRANCES =
    "[[layer]]\nname = \"far\"\nentrance = true\n"
    "image = { file = \"a.png\" }\n"
    "[[layer]]\nname = \"near\"\nentrance = true\n"
    "image = { file = \"b.png\" }\n";
constexpr std::string_view SECOND_ENTRANCE_REFUSAL =
    "layer[1].entrance: one layer at most is the entrance of a background";
constexpr std::string_view A_FILE_OUTSIDE_THE_ASSETS =
    "image = { file = \"../secret.png\" }\n";
constexpr std::string_view FILE_REFUSAL =
    "layer[0].image.file: a file is named by its asset id";
constexpr std::string_view WARPS_WRITTEN_AS_A_NUMBER =
    "motion = { warps = 1 }\nimage = { file = \"a.png\" }\n";
constexpr std::string_view A_PARALLAX_OF_FIVE =
    "motion = { parallax = 5 }\nimage = { file = \"a.png\" }\n";

/** @brief Particles whose keys do not go together, as particlesReading()
 * takes them, and what the error says of each. */
constexpr std::string_view A_POPULATION_OF_A_GROUP =
    "population = 1\ngroup = \"sky events\"\n"
    "area = { left = 0, top = 0, width = 1, height = 1 }\npixel = \"blue\"\n";
constexpr std::string_view NEITHER_POPULATION_NOR_GROUP =
    "area = { left = 0, top = 0, width = 1, height = 1 }\npixel = \"blue\"\n";
constexpr std::string_view EMISSION_REFUSAL =
    "layer[0].particles: particles hold either population or group";
constexpr std::string_view A_GROUP_NOTHING_DECLARES =
    "group = \"comets\"\n"
    "area = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nspeed = [1, 2]\n";
constexpr std::string_view UNKNOWN_GROUP_REFUSAL =
    "layer[0].particles.group: no [[group]] of the file has this name";
constexpr std::string_view A_PIXEL_AND_A_SPRITE =
    "population = 1\narea = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nsprites = [{ file = \"a.png\" }]\n";
constexpr std::string_view NEITHER_PIXEL_NOR_SPRITE =
    "population = 1\narea = { left = 0, top = 0, width = 1, height = 1 }\n";
constexpr std::string_view LOOK_REFUSAL =
    "layer[0].particles: particles hold either pixel or at least one sprite";
constexpr std::string_view A_TWINKLE_COLOR_WITHOUT_STEPS =
    "population = 1\narea = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\ntwinkle = \"pale_blue\"\n";
constexpr std::string_view A_PIXEL_THAT_TWINKLES_WITHOUT_A_COLOR =
    "population = 1\narea = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\ntwinkle_steps = 8\n";
constexpr std::string_view A_TWINKLE_COLOR_FOR_SPRITES =
    "population = 1\narea = { left = 0, top = 0, width = 1, height = 1 }\n"
    "sprites = [{ file = \"a.png\" }]\ntwinkle = \"blue\"\n"
    "twinkle_steps = 8\n";
constexpr std::string_view TWINKLE_REFUSAL =
    "layer[0].particles: twinkle, the color a pixel twinkles in";
constexpr std::string_view PARTICLES_THAT_STAY_FOR_GOOD =
    "group = \"sky events\"\n"
    "area = { left = 0, top = 0, width = 1, height = 1 }\npixel = \"blue\"\n";
constexpr std::string_view PARTICLES_THAT_MAY_STAY_FOR_GOOD =
    "group = \"sky events\"\n"
    "area = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nspeed = [0, 10]\n";
constexpr std::string_view PARTICLES_THAT_STAY_FOR_A_SECOND =
    "group = \"sky events\"\n"
    "area = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nlife_ms = [1000, 1000]\n";
constexpr std::string_view ENDLESS_REFUSAL =
    "layer[0].particles: the particles of a group end with life_ms";
constexpr std::string_view A_COLOR_OUTSIDE_THE_PALETTE =
    "population = 1\narea = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"crimson\"\n";
constexpr std::string_view PIXEL_KEY_NAME = "layer[0].particles.pixel";
constexpr std::string_view A_POPULATION_OF_NONE =
    "population = 0\narea = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\n";
constexpr std::string_view A_SPEED_WITH_ITS_FASTEST_FIRST =
    "group = \"sky events\"\n"
    "area = { left = 0, top = 0, width = 1, height = 1 }\n"
    "pixel = \"blue\"\nspeed = [10, 1]\n";

/** @brief Two groups of one name, and what the error says. */
constexpr std::string_view TWO_GROUPS_OF_ONE_NAME =
    "[[group]]\nname = \"sky events\"\nmost_alive = 4\nfirst_ms = 600\n"
    "every_ms = [800, 2600]\n"
    "[[group]]\nname = \"sky events\"\nmost_alive = 2\nfirst_ms = 0\n"
    "every_ms = [100, 200]\n"
    "[[layer]]\nname = \"layer\"\nimage = { file = \"a.png\" }\n";
constexpr std::string_view SHARED_GROUP_NAME_REFUSAL =
    "group[1].name: another [[group]] of the file has this name";

/** @brief The most layers a background holds, and one more. */
constexpr std::size_t MOST_LAYERS = 32;
constexpr std::size_t ONE_LAYER_TOO_MANY = 33;
constexpr std::string_view TOO_MANY_LAYERS_REFUSAL =
    "layer holds more than 32 tables";
