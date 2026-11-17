#pragma once

#include <cstddef>
#include <vector>
#include "SpeechLines.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A line a SpokenLine is saying: its lines, how many letters they
 * hold, the letter each word to voice starts at, how many of those words
 * were voiced, and how long it has been said. */
struct SaidLine {
  SpeechLines lines;
  std::size_t letters{};
  std::vector<std::size_t> wordStarts;
  std::size_t voicedWords{};
  engine::Duration saidFor{};
};

}  // namespace rtype::client
