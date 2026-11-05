#include "WorldFixtures.hpp"
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include "Pilot.hpp"
#include "World.hpp"

namespace rtype::game::testing {

std::optional<std::string_view> behaviourNameOf(World& world,
                                                std::uint32_t networkId) {
  const std::optional<Pilot> pilot = componentOf<Pilot>(world, networkId);
  if (!pilot.has_value()) {
    return std::nullopt;
  }
  return pilot->behaviour.get().name();
}

std::vector<std::int64_t> pilotSettingsOf(World& world,
                                          std::uint32_t networkId) {
  const std::optional<Pilot> pilot = componentOf<Pilot>(world, networkId);
  if (!pilot.has_value()) {
    return {};
  }
  return {pilot->settings.begin(), pilot->settings.end()};
}

}  // namespace rtype::game::testing
