#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The rhythm of the protocol, written here from the protocol page so
 * a wrong constant fails: a copy every 500 ms, given up after 5 s. Fifty
 * frames of 100 ms reach the give up; the line leaves at once, then at each
 * 500 ms before it, ten times. */
constexpr engine::Duration CHAT_INTERVAL = std::chrono::milliseconds(500);
constexpr engine::Duration SMALLEST_TIME = std::chrono::nanoseconds(1);
constexpr engine::Duration SHORT_FRAME = std::chrono::milliseconds(100);
constexpr int FRAMES_UNTIL_GIVING_UP = 50;
constexpr std::size_t COPIES_BEFORE_GIVING_UP = 10;

/** @brief Lines and pilots of the tests. */
constexpr std::string_view FIRST_TEXT = "hi";
constexpr std::string_view SECOND_TEXT = "ready?";
constexpr std::uint16_t FIRST_LINE = 1;
constexpr std::uint16_t SECOND_LINE = 2;
constexpr std::uint8_t NOVA_NUMBER = 2;
constexpr std::string_view NOVA = "NOVA";
constexpr std::uint16_t NOVA_LINE = 7;

/** @brief A text one byte longer than the protocol takes, and its letter. */
constexpr std::size_t TOO_LONG_TEXT_SIZE = 128;
constexpr char FILLER_LETTER = 'a';

/** @brief One line more than the chat keeps, the lines kept, and the oldest
 * kept then. */
constexpr int LINES_PAST_THE_LIMIT = 33;
constexpr std::size_t LINES_KEPT = 32;
constexpr std::string_view OLDEST_KEPT = "line 2";

/** @brief The lines taken at once: one in flight and three waiting. */
constexpr int LINES_TAKEN_AT_ONCE = 4;

}  // namespace rtype::client::testing
