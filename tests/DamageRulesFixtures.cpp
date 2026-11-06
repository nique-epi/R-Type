#include "DamageRulesFixtures.hpp"
#include <cstdint>
#include "DamageScene.hpp"
#include "EntityDestruction.hpp"

namespace rtype::game::testing {

Announcement removed(std::uint32_t networkId) {
  return EntityDestruction{.networkId = networkId,
                           .cause = DestructionCause::Removed};
}

Announcement killed(std::uint32_t networkId) {
  return EntityDestruction{.networkId = networkId,
                           .cause = DestructionCause::Killed};
}

}  // namespace rtype::game::testing
