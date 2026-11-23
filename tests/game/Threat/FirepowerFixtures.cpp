#include "FirepowerFixtures.hpp"
#include <cstddef>
#include "Gun.hpp"
#include "GunSheet.hpp"
#include "ShotPattern.hpp"

namespace rtype::game::testing {

GunSheet gunSheetFiring(const ShotPattern& pattern, std::size_t intervalTicks) {
  GunSheet gun;
  gun.intervalTicks = intervalTicks;
  gun.pattern = pattern;
  return gun;
}

Gun gunFiring(const ShotPattern& pattern, std::size_t intervalTicks) {
  return Gun{.intervalTicks = intervalTicks, .pattern = pattern};
}

}  // namespace rtype::game::testing
