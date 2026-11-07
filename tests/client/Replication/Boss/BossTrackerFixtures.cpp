#include "BossTrackerFixtures.hpp"
#include <cstdint>
#include "BossStatus.hpp"
#include "BossTrackerTestConstants.hpp"

namespace rtype::client::testing {

network::BossStatus fightingWith(std::uint16_t health) {
  return network::BossStatus{.bossEntityId = TRACKED_BOSS_ID,
                             .health = health,
                             .fullHealth = TRACKED_FULL_HEALTH,
                             .stage = network::BossStage::Fighting};
}

}  // namespace rtype::client::testing
