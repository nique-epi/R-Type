#include "EntitySheetFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "EntitySheet.hpp"

namespace rtype::game::testing {

std::string_view pilotBehaviourName(const EntitySheet& sheet) {
  if (!sheet.pilot.has_value()) {
    return {};
  }
  return sheet.pilot->behaviour.get().name();
}

std::vector<std::int64_t> settingsOf(const EntitySheet& sheet) {
  if (!sheet.pilot.has_value()) {
    return {};
  }
  return sheet.pilot->settings;
}

std::size_t intervalTicksOf(const EntitySheet& sheet) {
  if (!sheet.gun.has_value()) {
    return 0;
  }
  return sheet.gun->intervalTicks;
}

std::size_t firstShotTicksOf(const EntitySheet& sheet) {
  if (!sheet.gun.has_value()) {
    return 0;
  }
  return sheet.gun->firstShotTicks;
}

std::string projectileNameOf(const EntitySheet& sheet) {
  if (!sheet.gun.has_value()) {
    return {};
  }
  return sheet.gun->projectile;
}

}  // namespace rtype::game::testing
