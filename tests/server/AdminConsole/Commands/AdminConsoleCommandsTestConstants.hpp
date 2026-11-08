#pragma once

#include <array>
#include <cstddef>
#include <string_view>

/** @brief What close-game writes once game 3 in place 1 was told to stop. */
constexpr std::string_view GAME_THREE_TOLD_TO_STOP_LINE =
    "game 3 in place 1 told to stop: it leaves the list once its process "
    "ended";

/** @brief Lines the commands must refuse. */
constexpr std::string_view CLOSE_EMPTY_PLACE_LINE = "close-game 2";
constexpr std::string_view CLOSE_WITHOUT_PLACE_LINE = "close-game";
constexpr std::string_view CLOSE_TWO_PLACES_LINE = "close-game 1 2";
constexpr std::string_view CLOSE_WORD_PLACE_LINE = "close-game one";
/** @brief Places outside 0 to 65535 that wrap to place 1 when cut to the
 * 16 bits of a place: 65537 is 65536 + 1, -65535 is -65536 + 1. */
constexpr std::string_view CLOSE_PLACE_PAST_SIXTEEN_BITS_LINE =
    "close-game 65537";
constexpr std::string_view CLOSE_NEGATIVE_PLACE_LINE = "close-game -65535";
constexpr std::string_view GAMES_WITH_A_WORD_LINE = "games now";

/** @brief The pilots game 3 reports: one, PILOT1. kick names it in small
 * letters, since nicknames are compared without the case of ASCII letters,
 * and the order carries the nickname as typed. */
constexpr std::size_t REPORTED_PILOT_COUNT = 1;
constexpr std::string_view KICK_FIRST_PILOT_LINE = "kick pilot1";
constexpr std::string_view KICKED_NICKNAME = "pilot1";
constexpr std::string_view PILOT_ONE_TOLD_TO_LEAVE_LINE =
    "pilot1 told to leave game 3 in place 1";

/** @brief kick lines to refuse: a nickname no game holds, none, two, and one
 * of 17 bytes, one past the longest nickname. */
constexpr std::string_view KICK_NOBODY_LINE = "kick NOBODY";
constexpr std::string_view KICK_WITHOUT_NICKNAME_LINE = "kick";
constexpr std::string_view KICK_TWO_NICKNAMES_LINE = "kick PILOT1 PILOT2";
constexpr std::string_view KICK_TOO_LONG_NICKNAME_LINE =
    "kick ABCDEFGHIJKLMNOPQ";
/** @brief Why kick refuses the 17 bytes: the refusal of a word that is no
 * nickname, which the refusal of a pilot no game holds would not say. */
constexpr std::string_view NOT_A_NICKNAME_REASON =
    "'ABCDEFGHIJKLMNOPQ' is not a nickname";

/** @brief The help, and the names its lines start with, in their order. */
constexpr std::string_view HELP_LINE = "help";
constexpr std::array CONSOLE_COMMAND_NAMES{
    std::string_view{"close-game"}, std::string_view{"games"},
    std::string_view{"help"},       std::string_view{"kick"},
    std::string_view{"network"},    std::string_view{"players"}};

/** @brief The start of the line of the traffic, and the line of two
 * rejected datagrams. */
constexpr std::string_view TRAFFIC_LINE_START =
    "supervisor socket, traffic over ";
constexpr std::string_view TWO_REJECTED_LINE =
    "rejected since the start: 2 datagrams";
