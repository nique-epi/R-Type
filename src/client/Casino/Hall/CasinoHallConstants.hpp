#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include "BarState.hpp"
#include "CasinoBar.hpp"
#include "CasinoPaintingConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief How often the hall asks the master its rooms again. */
constexpr engine::Duration HALL_ROOMS_REFRESH = std::chrono::seconds(5);

/** @brief How long the hall fades to the night before it opens the room the
 * player walks to. */
constexpr engine::Duration HALL_FADE_TIME = std::chrono::milliseconds(360);

/**
 * @brief The bars of the hall, astride the bottom of the picture: CASHIER on
 * the left, its key LEFT_KEY, and ROULETTE on the right, RIGHT_KEY, the way
 * most players go; and BACK, MENU_CANCEL_KEY, never drawn, which walks back
 * to the doors.
 */
constexpr sf::Keyboard::Key HALL_LEFT_KEY = sf::Keyboard::Key::Left;
constexpr sf::Keyboard::Key HALL_RIGHT_KEY = sf::Keyboard::Key::Right;
constexpr CasinoBar HALL_CASHIER_BAR{.label = "CASHIER",
                                     .keyName = "<",
                                     .area = sf::IntRect({8, 252}, {92, 14}),
                                     .key = HALL_LEFT_KEY};
constexpr CasinoBar HALL_ROULETTE_BAR{
    .label = "ROULETTE",
    .keyName = ">",
    .area = sf::IntRect({372, 252}, {100, 14}),
    .key = HALL_RIGHT_KEY,
    .restState = BarState::Selected};
constexpr CasinoBar HALL_BACK_BAR{.label = BACK_LABEL,
                                  .keyName = CANCEL_KEY_NAME,
                                  .area = sf::IntRect{},
                                  .key = MENU_CANCEL_KEY};

/** @brief The arches of the picture: a click on the left one goes to the
 * cashier, on the right one to the roulette. */
constexpr sf::IntRect HALL_LEFT_ARCH({0, 40}, {30, 110});
constexpr sf::IntRect HALL_RIGHT_ARCH({406, 62}, {60, 80});

/** @brief The cartouche of the title, centered at the top of the picture. */
constexpr sf::IntRect HALL_TITLE_AREA({180, 4}, {120, 13});
constexpr sf::Vector2i HALL_TITLE_TOP_MIDDLE(240, 7);
constexpr const char* HALL_TITLE = "MAIN HALL";

/**
 * @brief The bulbs of the hall, lit in its picture: the pixels brighter than
 * HALL_BULB_*_FLOOR in the beam of the ceiling, the left arch and the apron of
 * the ring, ranked along a diagonal every HALL_BULB_RANK_WIDTH pixels; a bulb
 * out is HALL_UNLIT_BULB. The crystals of the two chandeliers are the same
 * bright pixels inside HALL_CHANDELIERS, which dim to HALL_DIM_CRYSTAL.
 */
constexpr std::uint8_t HALL_BULB_RED_FLOOR = 230;
constexpr std::uint8_t HALL_BULB_GREEN_FLOOR = 200;
constexpr std::uint8_t HALL_BULB_BLUE_FLOOR = 120;
constexpr std::array HALL_BULB_AREAS{sf::IntRect({0, 0}, {480, 32}),
                                     sf::IntRect({0, 30}, {32, 125}),
                                     sf::IntRect({120, 158}, {240, 15})};
constexpr int HALL_BULB_RANK_WIDTH = 4;
constexpr sf::Color HALL_UNLIT_BULB(CASINO_BRICK_RED.r, CASINO_BRICK_RED.g,
                                    CASINO_BRICK_RED.b, 217);
constexpr std::array HALL_CHANDELIERS{sf::IntRect({78, 4}, {42, 54}),
                                      sf::IntRect({358, 4}, {42, 54})};
constexpr sf::Color HALL_DIM_CRYSTAL(CASINO_DARK_GOLD.r, CASINO_DARK_GOLD.g,
                                     CASINO_DARK_GOLD.b, 128);

/** @brief The front ropes of the ring, between its front posts: kept from the
 * picture and painted again over the boxers, who spar behind them. A pixel
 * of rope is brighter than HALL_ROPE_LUMINANCE_FLOOR, or red. */
constexpr std::array HALL_FRONT_ROPE_ROWS{115, 116, 128, 129, 130};
constexpr int HALL_FRONT_ROPES_LEFT = 159;
constexpr int HALL_FRONT_ROPES_RIGHT = 321;
constexpr float HALL_ROPE_LUMINANCE_FLOOR = 100.0F;
constexpr std::uint8_t HALL_RED_ROPE_RED_FLOOR = 150;
constexpr std::uint8_t HALL_RED_ROPE_GREEN_CEILING = 100;

/**
 * @brief The crowd in front of the ring, between HALL_CROWD_TOP and
 * HALL_CROWD_BOTTOM: its dark mass, darker than HALL_CROWD_LUMINANCE_CEILING
 * but the red carpet, and the rim lights around it. Each spectator is the
 * stretch between two dips of the line of the heads, a dip at least
 * HALL_DIP_DEPTH under the highest head within HALL_DIP_REACH columns, and
 * HALL_SPECTATOR_WIDTH columns at least after the last one.
 */
constexpr int HALL_CROWD_TOP = 186;
constexpr int HALL_CROWD_BOTTOM = 217;
constexpr float HALL_CROWD_LUMINANCE_CEILING = 55.0F;
constexpr float HALL_CARPET_RED_OVER_BLUE = 1.6F;
constexpr float HALL_CARPET_GREEN_OVER_RED = 0.6F;
constexpr std::uint8_t HALL_CARPET_RED_FLOOR = 90;
constexpr int HALL_DIP_DEPTH = 4;
constexpr int HALL_DIP_REACH = 6;
constexpr int HALL_SPECTATOR_WIDTH = 10;

/**
 * @brief How a spectator hops: one pixel up while the sine of its beat is
 * above HALL_HOP_ONE_PIXEL, two above HALL_HOP_TWO_PIXELS; its beat runs
 * from HALL_SLOWEST_HOP to HALL_SLOWEST_HOP + HALL_HOP_SPEED_RANGE radians a
 * second, in HALL_HOP_SHARES steps.
 */
constexpr float HALL_HOP_ONE_PIXEL = 0.2F;
constexpr float HALL_HOP_TWO_PIXELS = 0.75F;
constexpr float HALL_SLOWEST_HOP = 4.0F;
constexpr float HALL_HOP_SPEED_RANGE = 4.0F;
constexpr std::uint32_t HALL_HOP_SHARES = 100;

/** @brief Camera flashes among the heads: one window of HALL_FLASH_TIME in
 * HALL_FLASH_ODDS holds one, somewhere in HALL_FLASH_AREA, a point and its
 * arms HALL_FLASH_SPAN pixels across. */
constexpr engine::Duration HALL_FLASH_TIME = std::chrono::milliseconds(140);
constexpr std::uint32_t HALL_FLASH_ODDS = 3;
constexpr sf::IntRect HALL_FLASH_AREA({10, 196}, {460, 40});
constexpr int HALL_FLASH_SPAN = 3;

/** @brief The boxers spar in a frame laid at HALL_BOXERS_TOP_LEFT, a frame
 * every HALL_BOXER_FRAME_TIME; on the frames of the punch, a spark flies
 * from HALL_SPARK_OFFSET inside the frame. */
constexpr sf::Vector2i HALL_BOXERS_TOP_LEFT(196, 96);
constexpr engine::Duration HALL_BOXER_FRAME_TIME =
    std::chrono::milliseconds(110);
constexpr std::array HALL_PUNCH_FRAMES{3, 4};
constexpr sf::Vector2i HALL_SPARK_OFFSET(52, 18);
constexpr std::array HALL_SPARK_GOLD_OFFSETS{
    sf::Vector2i(-2, -1), sf::Vector2i(2, -2), sf::Vector2i(1, 2)};

/**
 * @brief The spotlight over the ring, a cone from HALL_SPOT_TOP_HALF_WIDTH on
 * each side of HALL_SPOT_TOP_MIDDLE down to HALL_SPOT_FLOOR_HALF_WIDTH at row
 * HALL_SPOT_FLOOR, which breathes around HALL_SPOT_ALPHA by
 * HALL_SPOT_BREATH at HALL_SPOT_PACE radians a second.
 */
constexpr sf::Vector2i HALL_SPOT_TOP_MIDDLE(240, 14);
constexpr int HALL_SPOT_TOP_HALF_WIDTH = 14;
constexpr int HALL_SPOT_FLOOR_HALF_WIDTH = 58;
constexpr int HALL_SPOT_FLOOR = 152;
constexpr float HALL_SPOT_ALPHA = 18.0F;
constexpr float HALL_SPOT_BREATH = 5.0F;
constexpr float HALL_SPOT_PACE = 2.4F;

/**
 * @brief Two beams that sweep the ceiling from the edges of the ring, one
 * warm and one pale blue, HALL_BEAM_SWING radians either side of their rest,
 * over HALL_BEAM_PERIOD seconds there and back.
 */
constexpr sf::Vector2f HALL_LEFT_BEAM_ORIGIN(150.0F, 150.0F);
constexpr sf::Vector2f HALL_RIGHT_BEAM_ORIGIN(330.0F, 150.0F);
constexpr float HALL_BEAM_REST = 0.45F;
constexpr float HALL_BEAM_SWING = 0.35F;
constexpr float HALL_BEAM_PERIOD = 6.0F;
constexpr float HALL_BEAM_LENGTH = 170.0F;
constexpr float HALL_BEAM_HALF_SPREAD = 0.07F;
constexpr std::uint8_t HALL_BEAM_ALPHA = 18;

/** @brief The slot machines: one reel window at a time lights, every
 * HALL_SLOT_STEP, then they rest as long. */
constexpr std::array HALL_SLOT_WINDOWS{
    sf::IntRect({47, 94}, {12, 20}), sf::IntRect({62, 94}, {12, 20}),
    sf::IntRect({77, 94}, {12, 20}), sf::IntRect({92, 94}, {12, 20}),
    sf::IntRect({107, 94}, {12, 20})};
constexpr engine::Duration HALL_SLOT_STEP = std::chrono::milliseconds(260);
constexpr std::size_t HALL_SLOT_CYCLE_STEPS = HALL_SLOT_WINDOWS.size() * 2;
constexpr sf::Color HALL_LIT_SLOT(255, 211, 138, 72);

/** @brief The two chevrons that push toward the roulette, in the right arch:
 * HALL_CHEVRON_GAP apart, nudged a pixel every HALL_CHEVRON_STEP. */
constexpr sf::Vector2i HALL_CHEVRONS_TOP_LEFT(454, 108);
constexpr int HALL_CHEVRON_COUNT = 2;
constexpr int HALL_CHEVRON_GAP = 4;
constexpr int HALL_CHEVRON_NUDGE_STEPS = 2;
constexpr int HALL_CHEVRON_ROWS = 3;
constexpr engine::Duration HALL_CHEVRON_STEP = std::chrono::milliseconds(300);

/** @brief How many beats the twinkling crystals and the hopping crowd are
 * spread over, and how far apart two neighbouring beats start, in radians. */
constexpr std::size_t HALL_TWINKLE_GROUPS = 8;
constexpr float HALL_BEAT_SPREAD = 2.39996F;
constexpr float HALL_CRYSTAL_DIM_ABOVE = 0.5F;
constexpr float HALL_SLOWEST_CRYSTAL = 2.0F;
constexpr float HALL_CRYSTAL_SPEED_RANGE = 3.0F;

}  // namespace rtype::client
