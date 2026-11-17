#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A line said in a bubble shows a letter every SPOKEN_LETTER_TIME,
 * then stays SPOKEN_LINGER_TIME once whole before its bubble goes. */
constexpr engine::Duration SPOKEN_LETTER_TIME = std::chrono::milliseconds(30);
constexpr engine::Duration SPOKEN_LINGER_TIME = std::chrono::milliseconds(3000);

/** @brief The bubble: its text SPOKEN_BUBBLE_PADDING in from its border;
 * its tail, under its bottom edge, SPOKEN_TAIL_ROWS tall, narrowing by a
 * pixel on each side every row from SPOKEN_TAIL_HALF_WIDTH. */
constexpr int SPOKEN_BUBBLE_PADDING = 6;
constexpr int SPOKEN_TAIL_ROWS = 4;
constexpr int SPOKEN_TAIL_HALF_WIDTH = 3;

}  // namespace rtype::client
