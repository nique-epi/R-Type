#pragma once

#include <cstdint>

/** @brief The name of the logger the tests build on a journal. */
constexpr const char* JOURNAL_LOGGER_NAME = "plugin";

/** @brief What the logger of the tests writes: a text, then a number. */
constexpr const char* WRITTEN_TEXT = "took ";
constexpr int WRITTEN_NUMBER = 42;

/** @brief The body the journal must receive for WRITTEN_TEXT then
 * WRITTEN_NUMBER. */
constexpr const char* EXPECTED_BODY = "took 42";

/** @brief Text of a line below the level the refusing journal accepts. */
constexpr const char* REFUSED_TEXT = "not wanted";

/** @brief The event the tests write: its name, its message, and the values of
 * its fields. */
constexpr const char* JOINED_EVENT = "player.joined";
constexpr const char* JOINED_MESSAGE = "player 2 joined";
constexpr std::int64_t JOINED_PLAYER = 2;
constexpr const char* JOINED_ADDRESS = "192.168.1.20";
