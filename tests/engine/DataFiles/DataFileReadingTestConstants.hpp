#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "WholeNumberRange.hpp"

/** @brief The name the tests give their data file, which the errors show. */
constexpr std::string_view SHEET_FILE_NAME = "sheet.toml";

/** @brief Keys and tables the tests read, written out as a file names them. */
constexpr std::string_view GAMEPLAY_TABLE = "gameplay";
constexpr std::string_view HITBOX_TABLE = "hitbox";
constexpr std::string_view GUN_TABLE = "gun";
constexpr std::string_view SIDE_KEY = "side";
constexpr std::string_view PILOT_KEY = "pilot";
constexpr std::string_view HEALTH_KEY = "health";
constexpr std::string_view LOWEST_KEY = "lowest";
constexpr std::string_view HIGHEST_KEY = "highest";
constexpr std::string_view WIDTH_KEY = "width";

/** @brief The dotted names the errors and keyName() give those keys. */
constexpr std::string_view WIDTH_OF_THE_HITBOX = "gameplay.hitbox.width";
constexpr std::string_view NAME_OF_THE_GUN = "gameplay.gun";

/** @brief The only key a table of the tests may hold, and two choices of a
 * key. */
constexpr std::array<std::string_view, 1> SIDE_ONLY{SIDE_KEY};
constexpr std::array<std::string_view, 2> TWO_SIDES{"player", "enemy"};
constexpr std::size_t POSITION_OF_ENEMY = 1;

/** @brief The health a test accepts: 1 to 1000, both included. */
constexpr rtype::engine::WholeNumberRange HEALTH_RANGE{.lowest = 1,
                                                       .highest = 1000};
constexpr std::int64_t LOWEST_HEALTH = 1;
constexpr std::int64_t HIGHEST_HEALTH = 1000;

/**
 * @brief The files the tests read. In each, `side = ` puts its value on column
 * 8 and `health = ` on column 10; a key starts on column 1.
 */
constexpr std::string_view NOT_TOML_ON_LINE_2 = "side = \"enemy\"\npilot =\n";
constexpr std::string_view A_SIDE = "side = \"enemy\"\n";
constexpr std::string_view A_HITBOX = "[gameplay.hitbox]\nwidth = 96\n";
constexpr std::string_view GAMEPLAY_WITH_A_SIDE =
    "[gameplay]\nside = \"enemy\"\n";
constexpr std::string_view GAMEPLAY_WITH_A_MISSPELT_KEY =
    "[gameplay]\nside = \"enemy\"\nsidd = 1\n";
constexpr std::string_view HITBOX_THAT_IS_A_NUMBER = "[gameplay]\nhitbox = 3\n";
constexpr std::string_view GUN_THAT_IS_A_NUMBER = "[gameplay]\ngun = 3\n";
constexpr std::string_view GAMEPLAY_WITH_A_GUN =
    "[gameplay.gun]\ninterval_ms = 1500\n";
constexpr std::string_view BOTH_BOUNDS_OF_HEALTH =
    "[gameplay]\nlowest = 1\nhighest = 1000\n";
constexpr std::string_view NO_HEALTH = "[gameplay]\nhealth = 0\n";
constexpr std::string_view ONE_HEALTH_TOO_MANY = "[gameplay]\nhealth = 1001\n";
constexpr std::string_view HEALTH_AS_A_FLOAT = "[gameplay]\nhealth = 3.0\n";
constexpr std::string_view SIDE_AS_A_NUMBER = "[gameplay]\nside = 2\n";
constexpr std::string_view MISSPELT_SIDE = "[gameplay]\nside = \"enmy\"\n";

/** @brief An interval of 1500 ms, read as a duration, and the range it is
 * read in: 1 to 60000 ms. */
constexpr std::string_view AN_INTERVAL = "[gameplay]\ninterval_ms = 1500\n";
constexpr std::string_view INTERVAL_KEY = "interval_ms";
constexpr rtype::engine::WholeNumberRange INTERVAL_RANGE{.lowest = 1,
                                                         .highest = 60000};
constexpr std::chrono::milliseconds ONE_AND_A_HALF_SECONDS{1500};

/** @brief The text A_SIDE gives its side. */
constexpr std::string_view ENEMY = "enemy";

/** @brief What the errors say. */
constexpr std::string_view SYNTAX_ERROR_START = "sheet.toml:2:";
constexpr std::string_view MISSING_PILOT_TEXT = "gameplay.pilot is missing";
constexpr std::string_view MISSPELT_KEY_MESSAGE =
    "sheet.toml:3:1: unknown key gameplay.sidd";
constexpr std::string_view HITBOX_NOT_A_TABLE_TEXT =
    "gameplay.hitbox must be a table";
constexpr std::string_view NO_HEALTH_MESSAGE =
    "sheet.toml:2:10: gameplay.health must be an integer from 1 to 1000";
constexpr std::string_view SIDE_NOT_A_TEXT_MESSAGE =
    "sheet.toml:2:8: gameplay.side must be a text";
constexpr std::string_view MISSPELT_SIDE_MESSAGE =
    R"(sheet.toml:2:8: gameplay.side: unknown value "enmy"; known: player, enemy)";
constexpr std::string_view UNREADABLE_START = "cannot read the file ";

/** @brief A folder of data files: two sheets written out of their order, a
 * file of another kind, and a sheet in a sub-folder, which is not listed. */
constexpr std::string_view WEAVER_FILE = "weaver.toml";
constexpr std::string_view DRIFTER_FILE = "drifter.toml";
constexpr std::string_view NOTES_FILE = "notes.md";
constexpr std::string_view NESTED_SHIP_FILE = "nested/ship.toml";

/** @brief A folder no test writes. */
constexpr std::string_view MISSING_FOLDER = "no-such-folder/entities";

/** @brief A file whose bytes include a CRLF line ending, read back as is. */
constexpr std::string_view CRLF_FILE = "crlf.toml";
constexpr std::string_view CRLF_CONTENT = "side = \"enemy\"\r\n";
