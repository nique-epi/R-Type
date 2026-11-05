#pragma once

#include <chrono>
#include <optional>
#include "Endpoint.hpp"
#include "SenderAllowance.hpp"

namespace rtype::network {

/**
 * @brief One slot of RequestAllowances: the sender it holds, when it was last
 * seen, and what it spent. A slot no sender ever took has no lastSeen.
 */
struct AllowanceSlot {
  Endpoint sender;
  std::optional<std::chrono::nanoseconds> lastSeen;
  SenderAllowance allowance;
};

}  // namespace rtype::network
