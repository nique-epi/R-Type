#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"

namespace rtype::client::testing {

/** @brief The player, ACE, signed in with its account and its token, seated
 * beside LUNA. */
constexpr std::uint64_t TABLE_ACE_ACCOUNT = 7;
constexpr std::string_view TABLE_ACE_TOKEN =
    "rt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
constexpr std::string_view ACE_NAME = "ACE";
constexpr std::string_view LUNA_NAME = "LUNA";
constexpr std::string_view ACE_ROW = "ACE (YOU)";

/** @brief The room of the tests, as the master names it and as the tag of
 * its panel writes it. */
constexpr std::uint8_t NEBULA_LOUNGE = 2;
constexpr std::string_view NEBULA_LOUNGE_NAME = "Nebula Lounge";
constexpr std::string_view NEBULA_LOUNGE_TAG = "NEBULA LOUNGE";
constexpr std::uint8_t NEBULA_LOUNGE_SEATS = 8;

/** @brief The chips of the player, as the band writes them once a chip of 25
 * is laid; chips for one chip of 25 only; none; and what the wallet holds once
 * the master staked a chip of 25. */
constexpr std::uint32_t CHIPS_OF_ACE = 1'250;
constexpr std::string_view CHIPS_OF_ACE_TEXT = "1 250";
constexpr std::string_view CHIPS_ONCE_A_CHIP_IS_LAID = "1 225";
constexpr std::uint32_t CHIPS_FOR_ONE_CHIP_OF_25 = 30;
constexpr std::uint32_t NO_CHIPS = 0;
constexpr std::uint32_t CHIPS_LEFT_TO_ACE = 1'225;

/** @brief Two rounds of the room, one after the other. */
constexpr std::uint64_t A_ROUND = 41;
constexpr std::uint64_t THE_NEXT_ROUND = 42;

/** @brief The clock of the master as the bets open, how long its phases last,
 * and a second into a phase. */
constexpr std::chrono::milliseconds MASTER_TIME_AS_THE_BETS_OPEN{
    1'790'000'000'000};
constexpr std::chrono::seconds BETTING_TIME{12};
constexpr std::chrono::seconds SPINNING_TIME{6};
constexpr std::chrono::seconds RESULT_TIME{5};
constexpr std::chrono::seconds A_SECOND_INTO_THE_PHASE{1};

/** @brief The number of the answer of the master to the bets of the tests. */
constexpr std::uint64_t FIRST_BETS_ANSWER = 3;

/** @brief The status line as the bets open, then once the master clock moved
 * 1.5 s, a second begun counted whole; with no chip; a second after the wheel
 * started, 5 s before it stops; and when the master refused bets for too few
 * chips, and refused a seat in a full room. */
constexpr std::string_view BETS_CLOSE_IN_TWELVE =
    "PLACE YOUR BETS - CLOSE IN 12";
constexpr std::chrono::milliseconds A_SECOND_AND_A_HALF{1'500};
constexpr std::string_view BETS_CLOSE_IN_ELEVEN =
    "PLACE YOUR BETS - CLOSE IN 11";
constexpr std::string_view NO_CHIPS_STATUS = "NO CHIPS - SEE THE CASHIER";
constexpr std::string_view RESULT_IN_FIVE = "NO MORE BETS - RESULT IN 5";
constexpr std::string_view TOO_FEW_CHIPS_STATUS =
    "NOT ENOUGH CHIPS - SEE THE CASHIER";
constexpr std::string_view FULL_TABLE_STATUS = "THE TABLE IS FULL";

/** @brief 17 drawn, a black number: the stake of ACE, 25 on it, and what the
 * straight returns, 36 times it; the status line when it won, when it lost
 * its chip, and before the master answered its bets. */
constexpr std::uint8_t SEVENTEEN_WINS = 17;
constexpr std::uint32_t STAKE_OF_ACE = 25;
constexpr std::uint32_t PAYOUT_OF_A_STRAIGHT = 900;
constexpr std::string_view WON_STATUS = "17 BLACK  +900";
constexpr std::string_view LOST_STATUS = "17 BLACK  -25";
constexpr std::string_view DRAWN_STATUS = "17 BLACK";

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
constexpr std::string_view BALL_PICTURE = "sprites/casino/wheel_ball.png";

/** @brief The caption of the numbers drawn, and the word of the bar the
 * table no longer has. */
constexpr std::string_view LAST_WORD = "LAST";
constexpr std::string_view SPIN_WORD = "SPIN";

}  // namespace rtype::client::testing
