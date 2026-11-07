#pragma once

#include <cstdint>
#include "Position.hpp"

namespace rtype::client {

/** @brief One input the prediction sent and moved the ship for: its number,
 * whether it held shoot, and the ship as predicted right after it. */
struct PredictedCommand {
  std::uint32_t inputNumber{};
  bool shooting{false};
  game::Position shipAfter{};
};

}  // namespace rtype::client
