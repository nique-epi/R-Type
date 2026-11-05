#include "WorldFixtures.hpp"
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include "Entity.hpp"
#include "NetworkIdentity.hpp"
#include "Pilot.hpp"
#include "World.hpp"

namespace rtype::game::testing {

std::optional<std::string_view> behaviourNameOf(World& world,
                                                std::uint32_t networkId) {
  std::optional<std::string_view> name;
  world.components().forEach<NetworkIdentity, Pilot>(
      [&name, networkId](engine::Entity, const NetworkIdentity& identity,
                         const Pilot& pilot) {
        if (identity.networkId == networkId) {
          name = pilot.behaviour.get().name();
        }
      });
  return name;
}

std::vector<std::int64_t> pilotSettingsOf(World& world,
                                          std::uint32_t networkId) {
  std::vector<std::int64_t> settings;
  world.components().forEach<NetworkIdentity, Pilot>(
      [&settings, networkId](engine::Entity, const NetworkIdentity& identity,
                             const Pilot& pilot) {
        if (identity.networkId == networkId) {
          settings.assign(pilot.settings.begin(), pilot.settings.end());
        }
      });
  return settings;
}

}  // namespace rtype::game::testing
