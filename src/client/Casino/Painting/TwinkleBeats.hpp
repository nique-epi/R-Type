#pragma once

namespace rtype::client {

/** @brief How the points of light of a room twinkle: their beats run from
 * `slowest` to `slowest` + `speedRange` radians a second, and a point dims
 * while the sine of its beat is above `dimAbove`. */
struct TwinkleBeats {
  float slowest{};
  float speedRange{};
  float dimAbove{};
};

}  // namespace rtype::client
