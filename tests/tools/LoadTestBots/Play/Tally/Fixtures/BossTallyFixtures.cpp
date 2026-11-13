#include "BossTallyFixtures.hpp"
#include <cstdint>
#include "BossStatus.hpp"
#include "BossTallyTestConstants.hpp"
#include "EntityMessage.hpp"

namespace rtype::tools::testing {

namespace {

/** @returns The status of the boss in @p stage with @p health points. */
client::EntityMessage statusOf(network::BossStage stage, std::uint16_t health) {
  return network::BossStatus{.bossEntityId = BOSS_ENTITY_ID,
                             .health = health,
                             .fullHealth = FULL_BOSS_HEALTH,
                             .stage = stage};
}

}  // namespace

client::EntityMessage bossArriving() {
  return network::BossStatus{.bossEntityId = 0,
                             .health = 0,
                             .fullHealth = 0,
                             .stage = network::BossStage::Arriving};
}

client::EntityMessage bossFighting(std::uint16_t health) {
  return statusOf(network::BossStage::Fighting, health);
}

client::EntityMessage bossEnraged(std::uint16_t health) {
  return statusOf(network::BossStage::Enraged, health);
}

client::EntityMessage bossDefeated() {
  return statusOf(network::BossStage::Defeated, 0);
}

}  // namespace rtype::tools::testing
