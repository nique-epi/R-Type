#include "BossArrivalEffects.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include "BossArrivalConstants.hpp"
#include "CameraShake.hpp"
#include "ComponentRegistry.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "FlashGate.hpp"
#include "Invulnerability.hpp"
#include "NetworkIdentity.hpp"
#include "PixelEffects.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How many steps fall at @p sinceArrival at the latest. */
int joltsUpTo(engine::Duration sinceArrival) {
  if (sinceArrival < BOSS_ENTRY_START) {
    return 0;
  }
  return std::min(
      BOSS_JOLT_COUNT,
      static_cast<int>((sinceArrival - BOSS_ENTRY_START) / BOSS_JOLT_PERIOD) +
          1);
}

/** @returns The entity of network id @p bossId, or nothing when the client
 * has no such entity with a Position. */
std::optional<engine::Entity> bossNamed(engine::ComponentRegistry& components,
                                        std::uint32_t bossId) {
  std::optional<engine::Entity> boss;
  components.forEach<game::NetworkIdentity, game::Position>(
      [&boss, bossId](engine::Entity entity,
                      const game::NetworkIdentity& identity,
                      const game::Position&) {
        if (identity.networkId == bossId) {
          boss = entity;
        }
      });
  return boss;
}

}  // namespace

int joltsBetween(engine::Duration previous, engine::Duration now) {
  return joltsUpTo(now) - joltsUpTo(previous);
}

float rumbleTraumaAt(engine::Duration sinceArrival) {
  return sinceArrival >= BOSS_RUMBLE_START && sinceArrival < BOSS_RUMBLE_END
             ? BOSS_RUMBLE_TRAUMA
             : 0.0F;
}

BossArrivalEffects::BossArrivalEffects(EffectTools tools) : tools_(tools) {}

void BossArrivalEffects::advance(engine::Duration previous,
                                 engine::Duration now) const {
  const FlashGate& gate = tools_.gate;
  CameraShake& camera = tools_.camera;
  gate.shake(camera, rumbleTraumaAt(now) - camera.trauma());
  if (joltsBetween(previous, now) > 0) {
    gate.shake(camera, BOSS_JOLT_TRAUMA);
  }
}

void BossArrivalEffects::playRoar(std::uint32_t bossId) const {
  tools_.gate.get().shake(tools_.camera, BOSS_ROAR_TRAUMA);
  const std::optional<game::Position> boss = positionOf(bossId);
  if (!boss.has_value()) {
    return;
  }
  const game::Position mouth{.x = boss->x + BOSS_MOUTH_OFFSET.x,
                             .y = boss->y + BOSS_MOUTH_OFFSET.y};
  PixelEffects& pixels = tools_.pixels;
  pixels.wave({.shape = ROAR_BONE_RING,
               .center = mouth,
               .colors = ROAR_BONE_RAMP,
               .delay = engine::Duration::zero()});
  pixels.wave({.shape = ROAR_ORANGE_RING,
               .center = mouth,
               .colors = ROAR_ORANGE_RAMP,
               .delay = ROAR_ORANGE_RING_DELAY});
}

void BossArrivalEffects::shield(std::uint32_t bossId, bool shielded) const {
  engine::ComponentRegistry& components = tools_.components;
  const std::optional<engine::Entity> boss = bossNamed(components, bossId);
  if (!boss.has_value()) {
    return;
  }
  if (!shielded) {
    components.remove<Invulnerability>(*boss);
    return;
  }
  if (components.has<Invulnerability>(*boss)) {
    return;
  }
  components.add(*boss, Invulnerability{.elapsed = engine::Duration::zero(),
                                        .steady = !tools_.gate.get().flashes(),
                                        .outline = std::nullopt});
}

std::optional<game::Position> BossArrivalEffects::positionOf(
    std::uint32_t bossId) const {
  engine::ComponentRegistry& components = tools_.components;
  const std::optional<engine::Entity> boss = bossNamed(components, bossId);
  if (!boss.has_value()) {
    return std::nullopt;
  }
  return *components.get<game::Position>(*boss);
}

}  // namespace rtype::client
