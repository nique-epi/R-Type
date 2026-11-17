#pragma once

#include <chrono>
#include <cstddef>
#include <string>

/** @brief What a GatedJournalDestination kept of a line: its text, the time of
 * its call and how many lines were lost before it. */
struct ReceivedJournalLine {
  std::string text;
  std::chrono::system_clock::time_point time;
  std::size_t lostBefore{0};
};
