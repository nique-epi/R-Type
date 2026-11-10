#include "EntityWithAppearance.hpp"
#include <cstdint>
#include <optional>
#include "AppearanceReference.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "NetworkIdentity.hpp"

namespace rtype::client {

std::optional<engine::Entity> entityWithAppearance(
    engine::ComponentRegistry& components, std::uint32_t networkId) {
  std::optional<engine::Entity> found;
  components.forEach<game::NetworkIdentity, AppearanceReference>(
      [networkId, &found](engine::Entity entity,
                          const game::NetworkIdentity& identity,
                          const AppearanceReference& look) {
        if (identity.networkId == networkId && look.appearance != nullptr) {
          found = entity;
        }
      });
  return found;
}

}  // namespace rtype::client
