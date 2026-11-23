#include "Firepower.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include "CollisionConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntityType.hpp"
#include "EntityTypeNumber.hpp"
#include "Gun.hpp"
#include "GunSheet.hpp"
#include "PercentConstants.hpp"
#include "ShotPattern.hpp"
#include "TeamFirepower.hpp"
#include "ThreatConstants.hpp"
#include "WeaponSheet.hpp"

namespace rtype::game {

namespace {

/** @returns How far from the heading of the shot projectile @p index of
 * @p pattern flies, in degrees, as shotPlacement() lays it. */
float offsetDegreesOf(const ShotPattern& pattern, std::size_t index) {
  if (pattern.count <= 1) {
    return 0.0F;
  }
  return (-pattern.spreadDegrees * engine::HALF) +
         (pattern.spreadDegrees * static_cast<float>(index) /
          static_cast<float>(pattern.count - 1));
}

/** @returns How many projectiles of a shot of @p pattern fly within
 * FORWARD_CONE_DEGREES of its heading. */
std::size_t forwardProjectilesOf(const ShotPattern& pattern) {
  std::size_t forward = 0;
  for (std::size_t index = 0; index < pattern.count; ++index) {
    if (std::abs(offsetDegreesOf(pattern, index)) <= FORWARD_CONE_DEGREES) {
      ++forward;
    }
  }
  return forward;
}

/** @brief How often a gun fires forward: the projectiles of a shot that fly
 * forward, and the ticks between two shots. */
struct ForwardRate {
  std::size_t projectiles{0};
  std::size_t intervalTicks{0};
};

/** @returns @p held in percent of @p reference, rounded down;
 * WHOLE_PERCENT when @p reference fires nothing forward. */
int percentOf(ForwardRate held, ForwardRate reference) {
  if (reference.projectiles == 0 || held.intervalTicks == 0) {
    return WHOLE_PERCENT;
  }
  return static_cast<int>(held.projectiles * reference.intervalTicks *
                          static_cast<std::size_t>(WHOLE_PERCENT) /
                          (reference.projectiles * held.intervalTicks));
}

ForwardRate forwardRateOf(const GunSheet& gun) {
  return ForwardRate{.projectiles = forwardProjectilesOf(gun.pattern),
                     .intervalTicks = gun.intervalTicks};
}

}  // namespace

int firepowerPercentOf(const Gun& held, const GunSheet& sheetGun) {
  return percentOf(
      ForwardRate{.projectiles = forwardProjectilesOf(held.pattern),
                  .intervalTicks = held.intervalTicks},
      forwardRateOf(sheetGun));
}

int highestFirepowerPercentOf(const EntitySheet& ship) {
  if (!ship.gun.has_value()) {
    return WHOLE_PERCENT;
  }
  const ForwardRate reference = forwardRateOf(*ship.gun);
  int highest = WHOLE_PERCENT;
  for (const WeaponSheet& weapon : ship.weapons) {
    for (const GunSheet& level : weapon.levels) {
      highest = std::max(highest, percentOf(forwardRateOf(level), reference));
    }
  }
  return highest;
}

TeamFirepower teamFirepowerOf(engine::ComponentRegistry& components,
                              const EntitySheets& sheets) {
  int total = 0;
  int ships = 0;
  TeamFirepower team;
  components.forEach<Gun, EntityTypeNumber>(
      [&](engine::Entity body, const Gun& gun, const EntityTypeNumber& type) {
        const auto* const side = components.get<EntityType>(body);
        const EntitySheet* const sheet = sheets.numbered(type.number);
        if (side == nullptr || *side != EntityType::Player ||
            sheet == nullptr || !sheet->gun.has_value()) {
          return;
        }
        total += firepowerPercentOf(gun, *sheet->gun);
        ++ships;
        team.highestPercent =
            std::max(team.highestPercent, highestFirepowerPercentOf(*sheet));
      });
  if (ships > 0) {
    team.meanPercent = total / ships;
  }
  return team;
}

}  // namespace rtype::game
