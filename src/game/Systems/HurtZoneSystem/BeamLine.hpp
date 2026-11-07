#pragma once

#include "Direction.hpp"

namespace rtype::game {

/** @brief A beam laid on the playfield, in logical units: where it starts,
 * its way from there (a Direction of length 1), how long and how thick it
 * is. */
struct BeamLine {
  float startX{0.0F};
  float startY{0.0F};
  Direction heading{};
  float length{0.0F};
  float thickness{0.0F};
};

}  // namespace rtype::game
