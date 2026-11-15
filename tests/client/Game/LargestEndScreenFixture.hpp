#pragma once

#include <string>
#include <vector>
#include "EndScreenFixture.hpp"
#include "TimeConstants.hpp"

/** @brief An end screen after a defeat, its canvas at the largest text size.
 */
class LargestEndScreenFixture : public EndScreenFixture {
 protected:
  LargestEndScreenFixture();

  /** @returns The text of every line that runs past the left or the right
   * side of the picture at any frame of the next @p duration, frame by
   * frame, in the order written; a line with no letter typed yet writes
   * nothing and is left out. Lines the book carries up or down as it drops,
   * opens or leaves are not counted. */
  [[nodiscard]] std::vector<std::string> writingsOutSidewaysFor(
      rtype::engine::Duration duration);
};
