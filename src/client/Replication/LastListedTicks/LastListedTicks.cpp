#include "LastListedTicks.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "ReplicationConstants.hpp"

namespace rtype::client {

namespace {

/**
 * @returns true when @p tick comes UNLISTED_ENTITY_TICKS or more after
 * @p lastListedTick; false before, and for a tick older than
 * @p lastListedTick, which a caller following the contract never gives.
 */
bool unlistedLongEnough(std::uint32_t lastListedTick, std::uint32_t tick) {
  return tick >= lastListedTick &&
         tick - lastListedTick >= UNLISTED_ENTITY_TICKS;
}

}  // namespace

void LastListedTicks::listed(std::uint32_t networkId, std::uint32_t tick) {
  lastListedTickByNetworkId_[networkId] = tick;
}

void LastListedTicks::appeared(std::uint32_t networkId) {
  lastListedTickByNetworkId_[networkId] = newestTick_;
}

void LastListedTicks::forget(std::uint32_t networkId) {
  lastListedTickByNetworkId_.erase(networkId);
}

std::vector<std::uint32_t> LastListedTicks::takeUnlistedAt(std::uint32_t tick) {
  newestTick_ = tick;
  startWaitingEntitiesAt(tick);
  std::vector<std::uint32_t> unlisted = unlistedAt(tick);
  for (const std::uint32_t networkId : unlisted) {
    forget(networkId);
  }
  return unlisted;
}

void LastListedTicks::startWaitingEntitiesAt(std::uint32_t tick) {
  for (auto& [networkId, lastListedTick] : lastListedTickByNetworkId_) {
    if (!lastListedTick.has_value()) {
      lastListedTick = tick;
    }
  }
}

std::vector<std::uint32_t> LastListedTicks::unlistedAt(
    std::uint32_t tick) const {
  std::vector<std::uint32_t> unlisted;
  for (const auto& [networkId, lastListedTick] : lastListedTickByNetworkId_) {
    if (lastListedTick.has_value() &&
        unlistedLongEnough(*lastListedTick, tick)) {
      unlisted.push_back(networkId);
    }
  }
  return unlisted;
}

}  // namespace rtype::client
