#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "DeltaWorldState.hpp"

namespace rtype::client {

/**
 * @brief The parts of one state received so far: one slot per part, filled as
 * the parts arrive, `received` counting the filled ones.
 */
struct PendingDelta {
  std::uint32_t tick;
  std::uint8_t baseDistance;
  std::vector<std::optional<network::DeltaWorldState>> parts;
  std::size_t received;
};

}  // namespace rtype::client
