#pragma once

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
