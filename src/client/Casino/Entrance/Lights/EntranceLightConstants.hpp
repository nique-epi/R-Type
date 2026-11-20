#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include <cstdint>
#include "BeamPair.hpp"
#include "CasinoPaintingConstants.hpp"
#include "LightBeam.hpp"
#include "NeonCut.hpp"
#include "TimeConstants.hpp"
#include "TwinkleBeats.hpp"

namespace rtype::client {

/**
 * @brief The 777 pylon painted in the front of the casino, read once from
 * the picture with its doors closed: ENTRANCE_PYLON_AREA holds its script,
 * the oval of bulbs around its numerals, two diamonds on each side, two
 * columns of bulbs and the golden discs between them.
 */
constexpr sf::IntRect ENTRANCE_PYLON_AREA({180, 22}, {126, 104});
constexpr sf::IntRect ENTRANCE_SCRIPT_AREA({206, 26}, {74, 32});
constexpr sf::IntRect ENTRANCE_NUMERALS_AREA({214, 64}, {52, 24});
constexpr std::array ENTRANCE_LEFT_DIAMONDS{sf::IntRect({188, 32}, {18, 20}),
                                            sf::IntRect({180, 60}, {16, 22}),
                                            sf::IntRect({188, 88}, {16, 16})};
constexpr std::array ENTRANCE_RIGHT_DIAMONDS{sf::IntRect({274, 32}, {18, 20}),
                                             sf::IntRect({284, 60}, {16, 22}),
                                             sf::IntRect({272, 104}, {18, 20})};
constexpr std::array ENTRANCE_COLUMNS{sf::IntRect({190, 92}, {16, 32}),
                                      sf::IntRect({272, 92}, {18, 32})};
constexpr sf::IntRect ENTRANCE_DISCS_AREA({220, 92}, {42, 32});
constexpr int ENTRANCE_DISC_ROW_HEIGHT = 6;

/**
 * @brief The oval of bulbs around 777: centered at ENTRANCE_OVAL_MIDDLE,
 * ENTRANCE_OVAL_RADII wide and tall, its bulbs those whose distance to its
 * outline, in radii, lies between ENTRANCE_OVAL_INNER and
 * ENTRANCE_OVAL_OUTER, between rows ENTRANCE_OVAL_TOP and
 * ENTRANCE_OVAL_BOTTOM; ranked by angle in ENTRANCE_OVAL_RANKS around it.
 */
constexpr sf::Vector2f ENTRANCE_OVAL_MIDDLE(246.0F, 76.0F);
constexpr sf::Vector2f ENTRANCE_OVAL_RADII(45.0F, 17.0F);
constexpr float ENTRANCE_OVAL_INNER = 0.7F;
constexpr float ENTRANCE_OVAL_OUTER = 1.35F;
constexpr int ENTRANCE_OVAL_TOP = 58;
constexpr int ENTRANCE_OVAL_BOTTOM = 96;
constexpr int ENTRANCE_OVAL_RANKS = 48;

/** @brief The bulbs of a column rise one rank every ENTRANCE_COLUMN_RANK
 * rows, so the light climbs toward the oval. */
constexpr int ENTRANCE_COLUMN_RANK = 3;

/**
 * @brief The marquee: its bulbs, outside the letters of BYDO PALACE /
 * CASINO, which stay lit to stay readable, ranked by columns of
 * ENTRANCE_MARQUEE_RANK_WIDTH pixels, a bulb out ENTRANCE_UNLIT_MARQUEE.
 */
constexpr sf::IntRect ENTRANCE_MARQUEE_AREA({66, 126}, {348, 56});
constexpr sf::IntRect ENTRANCE_MARQUEE_LETTERS({158, 134}, {170, 36});
constexpr int ENTRANCE_MARQUEE_RANK_WIDTH = 3;
constexpr sf::Color ENTRANCE_UNLIT_MARQUEE(CASINO_BRICK_RED.r,
                                           CASINO_BRICK_RED.g,
                                           CASINO_BRICK_RED.b, 217);

/** @brief The diamonds blink in turn, the left then the right ones out,
 * every ENTRANCE_DIAMOND_STEP; a diamond out is ENTRANCE_UNLIT_DIAMOND. */
constexpr engine::Duration ENTRANCE_DIAMOND_STEP =
    std::chrono::milliseconds(500);
constexpr sf::Color ENTRANCE_UNLIT_DIAMOND(CASINO_BRICK_RED.r,
                                           CASINO_BRICK_RED.g,
                                           CASINO_BRICK_RED.b, 204);

/**
 * @brief A wave of light falls down the discs, two rows lit at a time, one
 * row lower every ENTRANCE_DISC_STEP, ENTRANCE_DISC_WAVE_PAUSE rows of dark
 * between two waves; a lit row glows ENTRANCE_DISC_GLOW, the others sink
 * under ENTRANCE_DISC_SHADE.
 */
constexpr engine::Duration ENTRANCE_DISC_STEP = std::chrono::milliseconds(140);
constexpr int ENTRANCE_DISC_WAVE_PAUSE = 4;
constexpr sf::Color ENTRANCE_DISC_GLOW(CASINO_WARM_CREAM.r, CASINO_WARM_CREAM.g,
                                       CASINO_WARM_CREAM.b, 89);
constexpr sf::Color ENTRANCE_DISC_SHADE(CASINO_DEEP_NAVY.r, CASINO_DEEP_NAVY.g,
                                        CASINO_DEEP_NAVY.b, 38);

/**
 * @brief The neon script cuts out now and then: as each wait of
 * ENTRANCE_NEON_CUTS begins in turn, its cuts, each ENTRANCE_NEON_CUT out
 * then as long lit, the script then under ENTRANCE_DARK_NEON; the waits come
 * round once they all passed.
 */
constexpr std::array ENTRANCE_NEON_CUTS{
    NeonCut{.wait = std::chrono::milliseconds(5200), .cuts = 2},
    NeonCut{.wait = std::chrono::milliseconds(7900), .cuts = 3},
    NeonCut{.wait = std::chrono::milliseconds(4400), .cuts = 2},
    NeonCut{.wait = std::chrono::milliseconds(8600), .cuts = 3},
    NeonCut{.wait = std::chrono::milliseconds(6100), .cuts = 2}};
constexpr engine::Duration ENTRANCE_NEON_CUT = std::chrono::milliseconds(60);
constexpr engine::Duration::rep ENTRANCE_NEON_CUT_PHASES = 2;
constexpr sf::Color ENTRANCE_DARK_NEON(CASINO_PLUM.r, CASINO_PLUM.g,
                                       CASINO_PLUM.b, 153);

/**
 * @brief The stars and the lit windows of the sky over the roof: pixels of
 * the sky above ENTRANCE_SKY_BOTTOM, outside the pylon and the silhouettes
 * of the roof, brighter than ENTRANCE_STAR_LUMINANCE and brighter than their
 * four neighbours by ENTRANCE_STAR_CONTRAST; one in two twinkles, dimming
 * under ENTRANCE_DIM_STAR on its own beat, as ENTRANCE_STAR_BEATS says.
 */
constexpr int ENTRANCE_SKY_BOTTOM = 126;
constexpr std::array ENTRANCE_ROOF_SILHOUETTES{
    sf::IntRect({135, 71}, {63, 55}), sf::IntRect({282, 71}, {63, 55})};
constexpr float ENTRANCE_STAR_LUMINANCE = 130.0F;
constexpr float ENTRANCE_STAR_CONTRAST = 50.0F;
constexpr int ENTRANCE_STAR_NEIGHBOURS = 4;
constexpr std::uint32_t ENTRANCE_TWINKLING_SHARE = 2;
constexpr sf::Color ENTRANCE_DIM_STAR(CASINO_DEEP_NAVY.r, CASINO_DEEP_NAVY.g,
                                      CASINO_DEEP_NAVY.b, 140);
constexpr TwinkleBeats ENTRANCE_STAR_BEATS{
    .slowest = 1.4F, .speedRange = 2.2F, .dimAbove = 0.35F};

/**
 * @brief Two searchlights sweep the sky from behind the palace, warm and
 * faint, 0.21 radians (12 degrees) either side of their rest, over 7 seconds
 * there and back.
 */
constexpr BeamPair ENTRANCE_SEARCHLIGHTS{
    .left =
        LightBeam{.origin = {110.0F, 130.0F},
                  .angle = -0.45F,
                  .length = 240.0F,
                  .halfSpread = 0.06F,
                  .color = sf::Color(CASINO_WARM_CREAM.r, CASINO_WARM_CREAM.g,
                                     CASINO_WARM_CREAM.b, 15)},
    .right =
        LightBeam{.origin = {380.0F, 130.0F},
                  .angle = 0.5F,
                  .length = 240.0F,
                  .halfSpread = 0.06F,
                  .color = sf::Color(CASINO_WARM_CREAM.r, CASINO_WARM_CREAM.g,
                                     CASINO_WARM_CREAM.b, 15)},
    .swing = 0.2094F,
    .period = 7.0F};

}  // namespace rtype::client
