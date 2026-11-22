#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief The settings of the casino of the route tests: two rooms of one
 * seat, whose bets stay open ten minutes a round, so a test finds them open;
 * and the line that turns the casino off. */
constexpr std::string_view TWO_ROOMS_LINE = "set casino-rooms 2";
constexpr std::string_view ONE_SEAT_LINE = "set casino-seats 1";
constexpr std::string_view LONG_BETS_LINE = "set casino-betting-seconds 600";
constexpr std::string_view SHORT_SPIN_LINE = "set casino-spinning-seconds 1";
constexpr std::string_view SHORT_RESULT_LINE = "set casino-result-seconds 1";
constexpr std::string_view CASINO_MODULE_OFF_LINE = "module casino off";
constexpr std::size_t TEST_ROOM_COUNT = 2;

/** @brief The least time the bets of the round a test bets on stay open, and
 * how often and how many times the test reads the room while it waits for
 * such a round. */
constexpr std::chrono::milliseconds BETS_OPEN_MARGIN{5'000};
constexpr std::chrono::milliseconds OPEN_BETS_POLL{250};
constexpr int OPEN_BETS_POLLS = 40;

/** @brief The paths of the casino of the tests. */
constexpr std::string_view ROOMS_URL = "/api/casino/rooms";
constexpr std::string_view FIRST_ROOM_URL = "/api/casino/rooms/1";
constexpr std::string_view NO_SUCH_ROOM_URL = "/api/casino/rooms/3";
constexpr std::string_view FIRST_SEAT_URL = "/api/casino/rooms/1/seats/me";
constexpr std::string_view NO_SUCH_SEAT_URL = "/api/casino/rooms/3/seats/me";
constexpr std::uint8_t FIRST_ROOM = 1;

/** @brief The members of the answers of the casino. */
constexpr std::string_view ROOMS_MEMBER_NAME = "rooms";
constexpr std::string_view NAME_MEMBER_NAME = "name";
constexpr std::string_view ROUND_MEMBER_NAME = "round";
constexpr std::string_view PHASE_MEMBER_NAME = "phase";
constexpr std::string_view CLOSES_AT_MEMBER_NAME = "closesAt";
constexpr std::string_view SERVER_TIME_MEMBER_NAME = "serverTime";
constexpr std::string_view WINNING_NUMBER_MEMBER_NAME = "winningNumber";
constexpr std::string_view PLAYERS_MEMBER_NAME = "players";
constexpr std::string_view STAKE_MEMBER_NAME = "stake";
constexpr std::string_view BETS_MEMBER_NAME = "bets";
constexpr std::string_view CASINO_CHIPS_MEMBER_NAME = "chips";
constexpr std::string_view BETTING_PHASE_TEXT = "betting";
constexpr std::string_view FIRST_ROOM_NAME = "Bydo Palace";

/** @brief A second account of the tests, and its password. */
constexpr std::string_view SECOND_PILOT_NICKNAME = "R9";
constexpr std::string_view SECOND_PILOT_PASSWORD = "arrowhead wave cannon";

/** @brief Bodies of bets: 10 chips on 19 and 25 on red (35 in all); 100 on
 * red; none; a split that crosses the end of a street; a bet of no chip; one
 * straight bet over the cap of a round; one more than the wallet holds; and a
 * kind no roulette has. */
constexpr std::string_view TWO_BETS_BODY =
    R"({"bets":[{"kind":"straight","firstNumber":19,"amount":10},)"
    R"({"kind":"red","firstNumber":0,"amount":25}]})";
constexpr std::int64_t TWO_BETS_STAKE = 35;
/** @brief The bets TWO_BETS_BODY lays, as the state of the room lists them
 * for the others seated. */
constexpr std::size_t TWO_BETS_COUNT = 2;
constexpr std::string_view HUNDRED_ON_RED_BODY =
    R"({"bets":[{"kind":"red","firstNumber":0,"amount":100}]})";
constexpr std::int64_t HUNDRED_STAKE = 100;
constexpr std::string_view NO_BETS_BODY = R"({"bets":[]})";
constexpr std::string_view CROSSING_SPLIT_BODY =
    R"({"bets":[{"kind":"split","firstNumber":3,"amount":10}]})";
constexpr std::string_view NO_CHIP_BODY =
    R"({"bets":[{"kind":"straight","firstNumber":7,"amount":0}]})";
constexpr std::string_view OVER_THE_CAP_BODY =
    R"({"bets":[{"kind":"red","firstNumber":0,"amount":2001}]})";
constexpr std::string_view MORE_THAN_THE_WALLET_BODY =
    R"({"bets":[{"kind":"red","firstNumber":0,"amount":501}]})";
constexpr std::string_view UNKNOWN_KIND_BET_BODY =
    R"({"bets":[{"kind":"triple","firstNumber":7,"amount":10}]})";
constexpr std::size_t ONE_BET_MORE_THAN_A_ROUND_TAKES = 33;
constexpr std::uint32_t ONE_CHIP = 1;

/** @brief The chips a wallet opens with, and what each stake leaves. */
constexpr std::int64_t OPENING_CHIPS = 500;
constexpr std::int64_t CHIPS_AFTER_TWO_BETS = 465;
constexpr std::int64_t CHIPS_AFTER_A_HUNDRED = 400;
