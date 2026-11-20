#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include "CasinoPaintingConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The old teller stands behind his grille, his frame at
 * CASHIER_TELLER_TOP_LEFT: counting his bills, a frame every
 * TELLER_COUNTING_FRAME_TIME round and round; serving once an exchange is
 * made, a frame every TELLER_SERVING_FRAME_TIME, then counting again. The
 * counter, from CASHIER_COUNTER_TOP down, is painted again over him.
 */
constexpr sf::Vector2i CASHIER_TELLER_TOP_LEFT(90, 107);
constexpr engine::Duration TELLER_COUNTING_FRAME_TIME =
    std::chrono::milliseconds(150);
constexpr engine::Duration TELLER_SERVING_FRAME_TIME =
    std::chrono::milliseconds(120);
constexpr int CASHIER_COUNTER_TOP = 182;

/**
 * @brief What the teller says, in a bubble over his head: TELLER_BUBBLE_WIDTH
 * wide, its bottom-left corner at TELLER_BUBBLE_BOTTOM_LEFT, its tail
 * TELLER_BUBBLE_TAIL from its left, pointing at his visor. He greets the
 * player as the cashier opens, thanks him once an exchange is served, and
 * apologizes when the master refuses one.
 */
constexpr int TELLER_BUBBLE_WIDTH = 132;
constexpr int TELLER_BUBBLE_TAIL = 30;
constexpr sf::Vector2i TELLER_BUBBLE_BOTTOM_LEFT(110, 108);
constexpr const char* TELLER_GREETING = "BUY OR CASH OUT, PILOT?";
constexpr const char* TELLER_SERVED_LINE = "THERE YOU GO, PILOT.";
constexpr const char* TELLER_REFUSED_LINE = "SORRY, PILOT. NO DEAL.";

/**
 * @brief The brass grille in front of the teller, read once from the
 * picture: the pixels of CASHIER_GRILLE_AREA redder than
 * CASHIER_BRASS_RED_FLOOR, greener than CASHIER_BRASS_GREEN_FLOOR, bluer than
 * nothing past CASHIER_BRASS_BLUE_CEILING, their red CASHIER_BRASS_WARMTH
 * above their blue.
 */
constexpr sf::IntRect CASHIER_GRILLE_AREA({116, 148}, {78, 50});
constexpr std::uint8_t CASHIER_BRASS_RED_FLOOR = 130;
constexpr std::uint8_t CASHIER_BRASS_GREEN_FLOOR = 80;
constexpr std::uint8_t CASHIER_BRASS_BLUE_CEILING = 90;
constexpr int CASHIER_BRASS_WARMTH = 50;

/**
 * @brief The gold of the open vault, which catches the light: the pixels of
 * CASHIER_VAULT_AREA redder than CASHIER_GOLD_RED_FLOOR, greener than
 * CASHIER_GOLD_GREEN_FLOOR, under CASHIER_GOLD_BLUE_CEILING in blue.
 * CASHIER_SPARKLES coins sparkle at a time, each CASHIER_SPARKLE_TIME, one
 * starting every CASHIER_SPARKLE_STAGGER, brightest at CASHIER_SPARKLE_PEAK
 * of its time: a white point and four arms of warm cream, at
 * CASHIER_SPARKLE_ARM_SHARE of its light.
 */
constexpr sf::IntRect CASHIER_VAULT_AREA({0, 30}, {150, 152});
constexpr std::uint8_t CASHIER_GOLD_RED_FLOOR = 210;
constexpr std::uint8_t CASHIER_GOLD_GREEN_FLOOR = 150;
constexpr std::uint8_t CASHIER_GOLD_BLUE_CEILING = 120;
constexpr std::size_t CASHIER_SPARKLES = 3;
constexpr engine::Duration CASHIER_SPARKLE_TIME =
    std::chrono::milliseconds(300);
constexpr engine::Duration CASHIER_SPARKLE_STAGGER =
    std::chrono::milliseconds(100);
constexpr float CASHIER_SPARKLE_ARM_SHARE = 0.6F;
constexpr float CASHIER_SPARKLE_PEAK = 0.5F;
constexpr std::array CASHIER_SPARKLE_ARMS{
    sf::Vector2i(1, 0), sf::Vector2i(-1, 0), sf::Vector2i(0, 1),
    sf::Vector2i(0, -1)};

/** @brief What blinks in the cage shows on one step in CASHIER_BLINK_PHASES:
 * the bills on top of the counting machine, shifted a pixel up every other
 * CASHIER_BILLS_STEP, and the light of the camera. */
constexpr engine::Duration::rep CASHIER_BLINK_PHASES = 2;
constexpr sf::IntRect CASHIER_BILLS_AREA({192, 150}, {32, 10});
constexpr engine::Duration CASHIER_BILLS_STEP = std::chrono::milliseconds(90);

/**
 * @brief A glint crosses the glass of the chip rack every
 * CASHIER_GLINT_PERIOD, in CASHIER_GLINT_TIME: a slanted band
 * CASHIER_GLINT_WIDTH wide, leaning CASHIER_GLINT_LEAN pixels from the top of
 * the glass to its bottom, starting CASHIER_GLINT_LEAD left of the glass and
 * running as far past it, CASHIER_GLINT_LEADS leads in all, in
 * CASHIER_GLINT_COLOR.
 */
constexpr sf::IntRect CASHIER_RACK_GLASS({226, 52}, {88, 100});
constexpr engine::Duration CASHIER_GLINT_PERIOD = std::chrono::seconds(4);
constexpr engine::Duration CASHIER_GLINT_TIME = std::chrono::milliseconds(1200);
constexpr int CASHIER_GLINT_WIDTH = 6;
constexpr int CASHIER_GLINT_LEAN = 36;
constexpr int CASHIER_GLINT_LEAD = 20;
constexpr int CASHIER_GLINT_LEADS = 2;
constexpr sf::Color CASHIER_GLINT_COLOR(CASINO_WARM_CREAM.r,
                                        CASINO_WARM_CREAM.g,
                                        CASINO_WARM_CREAM.b, 36);

/** @brief The red light of the camera, on every other
 * CASHIER_CAMERA_STEP. */
constexpr sf::Vector2i CASHIER_CAMERA_LIGHT(449, 21);
constexpr engine::Duration CASHIER_CAMERA_STEP = std::chrono::milliseconds(700);

/**
 * @brief The rate board on the wall, painted empty: EXCHANGE on top, then
 * each way and its rate, the way of the panel lit, the other dark.
 */
constexpr int CASHIER_BOARD_MIDDLE = 399;
constexpr int CASHIER_BOARD_TITLE_TOP = 48;
constexpr int CASHIER_BOARD_FIRST_ROW_TOP = 66;
constexpr int CASHIER_BOARD_ROW_SPACING = 24;
constexpr int CASHIER_BOARD_RATE_DROP = 10;
constexpr const char* CASHIER_BOARD_TITLE = "EXCHANGE";

}  // namespace rtype::client
