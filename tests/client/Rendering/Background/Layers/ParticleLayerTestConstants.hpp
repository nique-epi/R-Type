#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "LayerFrame.hpp"

/** @brief The repeat of the layers of stars of the sky: the 480 columns of
 * the background and 4 more out of sight on each side. */
constexpr float STAR_REPEAT = 488.0F;
constexpr float HIDDEN_ON_EACH_SIDE = 4.0F;
constexpr float NO_PARTICLE_REPEAT = 0.0F;

/** @brief The far stars of the sky, as particlesReading() takes them: 441
 * deep blue pixels that twinkle in blue one step in eight. */
constexpr std::string_view THE_FAR_STARS = R"(
population = 441
area = { left = 0, top = 0, width = 480, height = 270 }
pixel = "deep_blue"
twinkle = "blue"
twinkle_steps = 8
warp_trail = "pale_blue"
)";
constexpr std::size_t FAR_STARS_IN_THE_SKY = 441;
constexpr sf::Color DEEP_BLUE_STAR(38, 48, 94);
constexpr sf::Color BLUE_TWINKLE(74, 90, 168);
constexpr sf::Color PALE_BLUE_STREAK(143, 162, 230);

/** @brief One star alone, and one that leaves no streak in a warp. */
constexpr std::string_view ONE_STAR = R"(
population = 1
area = { left = 100, top = 50, width = 0, height = 0 }
pixel = "deep_blue"
twinkle = "blue"
twinkle_steps = 8
warp_trail = "pale_blue"
)";
constexpr std::string_view ONE_STAR_WITHOUT_A_STREAK = R"(
population = 1
area = { left = 100, top = 50, width = 0, height = 0 }
pixel = "deep_blue"
)";
constexpr sf::Vector2i WHERE_THE_STAR_LIES(100, 50);
constexpr sf::Vector2f PLACE_OF_THE_STAR(100.0F, 50.0F);
constexpr sf::Vector2f STAR_TEN_COLUMNS_LEFT(90.0F, 50.0F);
constexpr sf::Vector2f STAR_A_THOUSAND_COLUMNS_LEFT(-900.0F, 50.0F);
constexpr rtype::client::LayerFrame A_THOUSAND_COLUMNS_LEFT{.column = -1000};

/** @brief A twinkle step lasts a third of a second: 334 ms is just over one,
 * so eight of them reach eight steps in a row. */
constexpr std::chrono::milliseconds JUST_OVER_A_TWINKLE_STEP{334};
constexpr int STEPS_OF_A_TWINKLE_CYCLE = 8;

/** @brief One near star, drawn with a sprite of two frames, the second
 * while it twinkles, its middle on the star. */
constexpr std::string_view ONE_NEAR_STAR = R"(
population = 1
area = { left = 100, top = 50, width = 0, height = 0 }
twinkle_steps = 8
sprites = [
  { file = "backgrounds/space/near_star.png", frames = 2, anchor_x = 1, anchor_y = 1 },
]
)";
constexpr sf::Vector2i CORNER_OF_THE_NEAR_STAR(99, 49);
constexpr std::size_t FRAME_WITH_ITS_CROSS = 0;
constexpr std::size_t FRAME_WHILE_IT_TWINKLES = 1;

/** @brief A star that can come back anywhere on the height of the
 * background, and how far its layer goes in the test: 2000 columns, more
 * than four repeats. */
constexpr std::string_view ONE_STAR_OF_ANY_HEIGHT = R"(
population = 1
area = { left = 100, top = 0, width = 0, height = 270 }
pixel = "deep_blue"
)";
constexpr int COLUMNS_OF_A_LONG_FLIGHT = 2000;

/** @brief A thousand stars drawn with two sprites, the first four times as
 * likely as the second: four in five on average, between 750 and 850 here. */
constexpr std::string_view A_THOUSAND_STARS_OF_TWO_SPRITES = R"(
population = 1000
area = { left = 0, top = 0, width = 480, height = 270 }
sprites = [
  { file = "backgrounds/space/near_star.png", weight = 4 },
  { file = "backgrounds/space/warm_star.png", weight = 1 },
]
)";
constexpr std::size_t FIRST_SPRITE = 0;
constexpr int FEWEST_OF_THE_FIRST_SPRITE = 750;
constexpr int MOST_OF_THE_FIRST_SPRITE = 850;

/** @brief Particles of a group born on one point, (200, 100), that fly at 60
 * pixels per second: to the left when they give no angle, down at 90
 * degrees. */
constexpr std::string_view LEFTWARD_PARTICLES = R"(
group = "sky events"
area = { left = 200, top = 100, width = 0, height = 0 }
speed = [60, 60]
pixel = "star_white"
)";
constexpr std::string_view DOWNWARD_PARTICLES = R"(
group = "sky events"
area = { left = 200, top = 100, width = 0, height = 0 }
speed = [60, 60]
angle = [90, 90]
pixel = "star_white"
)";
constexpr sf::Vector2f BIRTH_POINT(200.0F, 100.0F);
constexpr float SIXTY_PIXELS = 60.0F;
constexpr float ROUNDING_OF_A_FLIGHT = 0.01F;

/** @brief Particles born anywhere in a box of 100 by 50 pixels at (300, 20),
 * and how many the test gives birth to. */
constexpr std::string_view PARTICLES_BORN_IN_A_BOX = R"(
group = "sky events"
area = { left = 300, top = 20, width = 100, height = 50 }
speed = [60, 60]
pixel = "star_white"
)";
constexpr float LEFT_OF_THE_BOX = 300.0F;
constexpr float RIGHT_OF_THE_BOX = 400.0F;
constexpr float TOP_OF_THE_BOX = 20.0F;
constexpr float BOTTOM_OF_THE_BOX = 70.0F;
constexpr std::size_t A_HUNDRED_BIRTHS = 100;

/** @brief Particles that drift up or down by at most 9 pixels per second. */
constexpr std::string_view DRIFTING_PARTICLES = R"(
group = "sky events"
area = { left = 200, top = 100, width = 0, height = 0 }
speed = [60, 60]
drift = 9
pixel = "star_white"
)";
constexpr float MOST_DRIFT = 9.0F;

/** @brief A flare: it stays where it is born and lives one second, its four
 * frames spread over its life. */
constexpr std::string_view FLARES_OF_ONE_SECOND = R"(
group = "sky events"
area = { left = 200, top = 100, width = 0, height = 0 }
life_ms = [1000, 1000]
sprites = [
  { file = "backgrounds/space/flare.png", frames = 4, anchor_x = 3, anchor_y = 3 },
]
)";
constexpr std::chrono::milliseconds JUST_SHORT_OF_A_SECOND{990};
constexpr std::chrono::milliseconds PAST_A_SECOND{20};
constexpr std::chrono::milliseconds A_TENTH_OF_A_SECOND{100};
constexpr std::chrono::milliseconds HALF_A_SECOND{500};
constexpr std::size_t FIRST_FRAME_OF_THE_FLARE = 0;
constexpr std::size_t THIRD_FRAME_OF_THE_FLARE = 2;
constexpr sf::Vector2i CORNER_OF_THE_FLARE(197, 97);

/** @brief A shooting star that lives 100 ms, with a trail of 12 pixels that
 * grows 180 pixels per second and shrinks 150: grown after 67 ms, it takes
 * 80 ms to shrink once the star has lived, so the star ends 180 ms in: it
 * still shows after 15 frames of 10 ms, and has ended after 5 more. */
constexpr std::string_view SHOOTING_STARS_OF_A_TENTH = R"(
group = "sky events"
area = { left = 200, top = 100, width = 0, height = 0 }
speed = [60, 60]
life_ms = [100, 100]
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
constexpr int FRAMES_UNTIL_ITS_TRAIL_SHRINKS = 15;
constexpr int FRAMES_UNTIL_ITS_TRAIL_HAS_SHRUNK = 5;

/** @brief A shooting star whose trail of 4 pixels is grown at once: after
 * 10 ms it flew 0.6 pixel to the left of (200, 100), so it is painted on
 * (199, 100), white, then three pixels of trail to its right, the first
 * white and the two beyond the half of the trail deep blue. */
constexpr std::string_view SHOOTING_STARS_WITH_A_SHORT_TRAIL = R"(
group = "sky events"
area = { left = 200, top = 100, width = 0, height = 0 }
speed = [60, 60]
life_ms = [1000, 1000]
pixel = "star_white"

[layer.particles.trail]
longest = 4
grow = 1000
shrink = 150
stops = [
  { until = 0.5, color = "star_white" },
  { until = 1, color = "deep_blue" },
]
)";
constexpr std::chrono::milliseconds TEN_MILLISECONDS{10};
constexpr sf::Color WHITE_STAR(238, 241, 255);
constexpr std::size_t PIXELS_OF_THE_SHORT_TRAIL = 4;
constexpr sf::Vector2i FIRST_PIXEL_OF_THE_TRAIL(200, 100);
constexpr sf::Vector2i LAST_PIXEL_OF_THE_TRAIL(202, 100);
constexpr std::size_t LAST_OF_THE_TRAIL = 2;
constexpr sf::Vector2i HEAD_OF_THE_SHOOTING_STAR(199, 100);

/** @brief An asteroid born 6 pixels right of the background, on the column
 * 486, that flies left at 100 pixels per second and ends 6 pixels left of
 * it: 492 pixels, flown in 4.92 s. Its four frames loop at 4 per second. */
constexpr std::string_view ASTEROIDS_ACROSS_THE_SKY = R"(
group = "sky events"
area = { left = 486, top = 100, width = 0, height = 0 }
speed = [100, 100]
margin = 6
sprites = [
  { file = "backgrounds/space/asteroid.png", frames = 4, fps = [4, 4] },
]
)";
constexpr std::chrono::milliseconds JUST_SHORT_OF_THE_CROSSING{4900};
constexpr std::chrono::milliseconds PAST_THE_CROSSING{50};
constexpr std::chrono::milliseconds ONE_FRAME_OF_THE_SPIN{250};
constexpr std::size_t FRAMES_OF_THE_ASTEROID = 4;
