#pragma once

#include <cstdint>

namespace rtype::engine {

/** @brief Identifies one subscription of an EventBus; never reused. */
struct SubscriptionHandle {
  std::uint64_t identifier;

  bool operator==(const SubscriptionHandle&) const = default;
};

}  // namespace rtype::engine
