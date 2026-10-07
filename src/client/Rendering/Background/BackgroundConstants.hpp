#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <ratio>
#include <string_view>
#include "PlayfieldConstants.hpp"
#include "Star.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Size of one background pixel, in logical units of the playfield.
 *
 * The background is painted at a quarter of the playfield resolution, then
 * enlarged over it: see ScrollingBackground for how its pixels stay sharp.
 */
constexpr float BACKGROUND_PIXEL_SIZE = 4.0F;

/** @brief Size of the background, in background pixels. */
constexpr float BACKGROUND_WIDTH =
    game::PLAYFIELD_WIDTH / BACKGROUND_PIXEL_SIZE;
constexpr float BACKGROUND_HEIGHT =
    game::PLAYFIELD_HEIGHT / BACKGROUND_PIXEL_SIZE;
constexpr sf::Vector2u BACKGROUND_SIZE(
    static_cast<unsigned int>(BACKGROUND_WIDTH),
    static_cast<unsigned int>(BACKGROUND_HEIGHT));

/**
 * @brief Longest time the background moves by in one frame.
 *
 * After a stall, such as the window being dragged, the sky resumes where it
 * was instead of jumping ahead.
 */
constexpr engine::Duration MAXIMUM_BACKGROUND_STEP =
    std::chrono::milliseconds(50);

/**
 * @brief Offset from the top-left corner of a pixel to its center: a point
 * drawn there lights exactly that pixel.
 */
constexpr sf::Vector2f PIXEL_CENTER(0.5F, 0.5F);

/** @brief Offsets to the pixels around a pixel. */
constexpr sf::Vector2i PIXEL_ABOVE(0, -1);
constexpr sf::Vector2i PIXEL_BELOW(0, 1);
constexpr std::array CROSS_DIRECTIONS{sf::Vector2i(-1, 0), sf::Vector2i(1, 0),
                                      PIXEL_ABOVE, PIXEL_BELOW};
constexpr std::array DIAGONAL_DIRECTIONS{
    sf::Vector2i(1, 1), sf::Vector2i(-1, -1), sf::Vector2i(1, -1),
    sf::Vector2i(-1, 1)};

/**
 * @brief The three star layers, in the order of StarDepth: the farther a
 * layer, the more stars it holds and the slower they go.
 */
constexpr std::array STAR_LAYERS{
    StarLayer{.depth = StarDepth::Far, .count = 441, .speed = 4.5F},
    StarLayer{.depth = StarDepth::Middle, .count = 90, .speed = 12.0F},
    StarLayer{.depth = StarDepth::Near, .count = 36, .speed = 30.0F},
};

/**
 * @brief A star that goes further left than this margin comes back as far
 * beyond the right edge, at a new height: it travels STAR_WRAP_DISTANCE.
 */
constexpr float STAR_WRAP_MARGIN = 4.0F;
constexpr float STAR_WRAP_DISTANCE =
    BACKGROUND_WIDTH + (2.0F * STAR_WRAP_MARGIN);

/** @brief Chance that a star is warm: a warm near star is drawn gold. */
constexpr float WARM_STAR_CHANCE = 0.2F;

/**
 * @brief Twinkling: time runs in steps of a third of a second, and each star
 * twinkles during one step out of STAR_TWINKLE_CYCLE_STEPS.
 */
using StarTwinkleStep = std::chrono::duration<std::int64_t, std::ratio<1, 3>>;
constexpr std::int64_t STAR_TWINKLE_CYCLE_STEPS = 8;

/**
 * @brief Planet image: a sphere PLANET_RADIUS pixels in radius, centered in an
 * image wide enough for its ring, which spans 1.9 radii on each side.
 */
constexpr int PLANET_RADIUS = 39;
constexpr int PLANET_HALF_WIDTH = 2 * PLANET_RADIUS;
constexpr int PLANET_IMAGE_EXTRA_ROWS = 7;
constexpr sf::Vector2u PLANET_IMAGE_SIZE(
    static_cast<unsigned int>((2 * PLANET_HALF_WIDTH) + 1),
    static_cast<unsigned int>((2 * PLANET_RADIUS) + PLANET_IMAGE_EXTRA_ROWS));
constexpr sf::Vector2i PLANET_CENTER(PLANET_HALF_WIDTH,
                                     static_cast<int>(PLANET_IMAGE_SIZE.y) / 2);

/**
 * @brief Light on the planet: from the left and the top across the sphere,
 * and from the viewer. Kept below 1 so dithering never goes past the
 * lightest shade.
 */
constexpr sf::Vector2f PLANET_LIGHT_ACROSS(-0.55F, -0.45F);
constexpr float PLANET_LIGHT_FROM_VIEWER = 0.7F;
constexpr float PLANET_BRIGHTEST_LIGHT = 0.999F;

/**
 * @brief 2 x 2 Bayer matrix: thresholds, in quarters, that mix two
 * neighbouring shades of the planet into a checkerboard.
 */
constexpr std::array PLANET_DITHER_MATRIX{std::array{0, 2}, std::array{3, 1}};
constexpr float PLANET_DITHER_LEVELS = 4.0F;

/**
 * @brief Planet ring: half width and half height in planet radii, tilt in
 * pixels, and angle between two of its points, in radians.
 */
constexpr float PLANET_RING_WIDTH_RADII = 1.9F;
constexpr float PLANET_RING_HEIGHT_RADII = 0.32F;
constexpr float PLANET_RING_TILT = 2.0F;
constexpr float PLANET_RING_ANGLE_STEP = 0.01F;
constexpr float FULL_TURN = 2.0F * std::numbers::pi_v<float>;

/**
 * @brief Planet movement: speed towards the left in pixels per second, start
 * and height as shares of the background, and how far beyond the right edge
 * it comes back once it has left on the left.
 */
constexpr float PLANET_SPEED = 1.8F;
constexpr float PLANET_START_SHARE_OF_WIDTH = 0.6F;
constexpr float PLANET_TOP_SHARE_OF_HEIGHT = 0.16F;
constexpr float PLANET_REENTRY_MARGIN = 10.0F;

/** @brief Number of colors along the tail of a shooting star or a comet. */
constexpr std::size_t TAIL_GRADIENT_STOP_COUNT = 4;

/**
 * @brief Halo: the background is blurred with a Gaussian of this standard
 * deviation, in background pixels, sampled over HALO_KERNEL_DEVIATIONS
 * standard deviations on each side.
 *
 * The design blurs by 4 pixels of a preview 640 pixels wide, which shows the
 * 480-pixel background: 3 background pixels.
 */
constexpr float HALO_STANDARD_DEVIATION = 3.0F;
constexpr float HALO_KERNEL_DEVIATIONS = 3.0F;
constexpr int HALO_KERNEL_RADIUS =
    static_cast<int>(HALO_KERNEL_DEVIATIONS * HALO_STANDARD_DEVIATION);

/**
 * @brief Fragment shader of one blur pass along `direction`, one texel long.
 * It is compiled after HALO_KERNEL_RADIUS_DEFINITION and HALO_KERNEL_RADIUS.
 */
constexpr std::string_view HALO_KERNEL_RADIUS_DEFINITION =
    "#define KERNEL_RADIUS ";
constexpr std::string_view HALO_SOURCE_UNIFORM = "source";
constexpr std::string_view HALO_DIRECTION_UNIFORM = "direction";
constexpr std::string_view HALO_DEVIATION_UNIFORM = "standardDeviation";
constexpr std::string_view HALO_FRAGMENT_SHADER = R"glsl(
uniform sampler2D source;
uniform vec2 direction;
uniform float standardDeviation;

void main()
{
    vec4 sum = vec4(0.0);
    float weightSum = 0.0;
    for (int offset = -KERNEL_RADIUS; offset <= KERNEL_RADIUS; ++offset)
    {
        float pixelOffset = float(offset);
        float weight = exp(-(pixelOffset * pixelOffset)
                           / (2.0 * standardDeviation * standardDeviation));
        sum += texture2D(source, gl_TexCoord[0].xy + direction * pixelOffset)
               * weight;
        weightSum += weight;
    }
    gl_FragColor = gl_Color * (sum / weightSum);
}
)glsl";

constexpr std::string_view BACKGROUND_LOG_MODULE_NAME = "Background";

}  // namespace rtype::client
