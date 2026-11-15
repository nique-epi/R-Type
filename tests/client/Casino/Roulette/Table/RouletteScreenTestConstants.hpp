#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "RouletteBets.hpp"

namespace rtype::client::testing {

/** @brief The player, ACE, number 1, seated beside LUNA, number 2. */
constexpr std::uint8_t ACE_NUMBER = 1;
constexpr std::string_view ACE_NAME = "ACE";
constexpr std::uint8_t LUNA_NUMBER = 2;
constexpr std::string_view LUNA_NAME = "LUNA";
constexpr std::string_view ACE_ROW = "ACE (YOU)";

/** @brief The chips of the player, as the band writes them once a chip of 25
 * is laid; chips for one chip of 25 only; none. */
constexpr std::uint32_t CHIPS_OF_ACE = 1'250;
constexpr std::string_view CHIPS_ONCE_A_CHIP_IS_LAID = "1 225";
constexpr std::uint32_t CHIPS_FOR_ONE_CHIP_OF_25 = 30;
constexpr std::uint32_t NO_CHIPS = 0;
/** @brief What the settings hold once the round is settled, the chip lost or
 * won: the band keeps showing what the player held when the bets closed. */
constexpr std::uint32_t CHIPS_ONCE_SETTLED = 2'125;

/** @brief Two rounds of the table, one after the other; the bets open for
 * 720 ticks, 12 s at 60 ticks a second. */
constexpr std::uint16_t A_ROUND = 41;
constexpr std::uint16_t THE_NEXT_ROUND = 42;
constexpr std::uint16_t TWELVE_SECONDS_OF_TICKS = 720;

/** @brief The status line as the bets open, then 1.5 s later, a second
 * begun counted whole; with no chip; while the wheel turns. */
constexpr std::string_view BETS_CLOSE_IN_TWELVE =
    "PLACE YOUR BETS - CLOSE IN 12";
constexpr std::chrono::milliseconds A_SECOND_AND_A_HALF{1'500};
constexpr std::string_view BETS_CLOSE_IN_ELEVEN =
    "PLACE YOUR BETS - CLOSE IN 11";
constexpr std::string_view NO_CHIPS_STATUS = "NO CHIPS - SEE THE CASHIER";
constexpr std::string_view NO_MORE_BETS_STATUS = "NO MORE BETS";

/** @brief 17 drawn, a black number: the stake of ACE, 25 on it, and what the
 * straight returns, 36 times it; the status line when it won, and when it
 * lost its chip. */
constexpr std::uint8_t SEVENTEEN_WINS = 17;
constexpr std::uint32_t STAKE_OF_ACE = 25;
constexpr std::uint32_t PAYOUT_OF_A_STRAIGHT = 900;
constexpr std::string_view WON_STATUS = "17 BLACK  +900";
constexpr std::string_view LOST_STATUS = "17 BLACK  -25";

/** @brief A chip of 25 on 17, where the table opens its cursor; of 500 there;
 * of 1 there; and of 25 on the line between 17 and 20. */
constexpr network::RouletteBet TWENTY_FIVE_ON_17{
    .kind = network::RouletteBetKind::Straight,
    .firstNumber = 17,
    .amount = 25};
constexpr network::RouletteBet FIVE_HUNDRED_ON_17{
    .kind = network::RouletteBetKind::Straight,
    .firstNumber = 17,
    .amount = 500};
constexpr network::RouletteBet ONE_ON_17{
    .kind = network::RouletteBetKind::Straight, .firstNumber = 17, .amount = 1};
constexpr network::RouletteBet TWENTY_FIVE_ACROSS_17_AND_20{
    .kind = network::RouletteBetKind::SplitAcross,
    .firstNumber = 17,
    .amount = 25};

/** @brief A pixel inside the chip of 1, the first of the band, which lies at
 * x 150, y 235, 23 x 24. */
constexpr sf::Vector2i INSIDE_THE_CHIP_OF_ONE(160, 245);

/** @brief The frame of the turns of the rotor that brings 17 to the top, 58,
 * on the sheet: ten frames of 125 to a row, so the ninth of the sixth row. */
constexpr sf::IntRect FRAME_THAT_BRINGS_17_UP({1000, 625}, {125, 125});
constexpr std::string_view ROTOR_TURNS_SHEET =
    "sprites/casino/wheel_rotor_turn.png";

/** @brief The caption of the numbers drawn. */
constexpr std::string_view LAST_WORD = "LAST";

}  // namespace rtype::client::testing
