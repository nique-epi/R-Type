#pragma once

#include <cstdint>

namespace rtype::engine {

/** @brief Identifies one subscription to an EventBus, or to any list of
 * callbacks that hands out handles; never reused by what handed it out. */
struct SubscriptionHandle {
  std::uint64_t identifier;

  bool operator==(const SubscriptionHandle&) const = default;
};

}  // namespace rtype::engine
