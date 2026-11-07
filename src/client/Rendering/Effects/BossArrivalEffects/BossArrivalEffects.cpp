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

/** @returns Where the mouth of the entity of network id @p bossId is, or
 * nothing when the client has no such entity with a Position. */
std::optional<game::Position> mouthOf(engine::ComponentRegistry& components,
                                      std::uint32_t bossId) {
  std::optional<game::Position> mouth;
  components.forEach<game::NetworkIdentity, game::Position>(
      [&mouth, bossId](engine::Entity, const game::NetworkIdentity& identity,
                       const game::Position& position) {
        if (identity.networkId == bossId) {
          mouth = game::Position{.x = position.x + BOSS_MOUTH_OFFSET.x,
                                 .y = position.y + BOSS_MOUTH_OFFSET.y};
        }
      });
  return mouth;
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
                                 engine::Duration now) {
  const FlashGate& gate = tools_.gate;
  CameraShake& camera = tools_.camera;
  gate.shake(camera, rumbleTraumaAt(now) - camera.trauma());
  if (joltsBetween(previous, now) > 0) {
    gate.shake(camera, BOSS_JOLT_TRAUMA);
  }
}

void BossArrivalEffects::playRoar(std::uint32_t bossId) {
  tools_.gate.get().shake(tools_.camera, BOSS_ROAR_TRAUMA);
  const std::optional<game::Position> mouth =
      mouthOf(tools_.components, bossId);
  if (!mouth.has_value()) {
    return;
  }
  PixelEffects& pixels = tools_.pixels;
  pixels.wave({.shape = ROAR_BONE_RING,
               .center = *mouth,
               .colors = ROAR_BONE_RAMP,
               .delay = engine::Duration::zero()});
  pixels.wave({.shape = ROAR_ORANGE_RING,
               .center = *mouth,
               .colors = ROAR_ORANGE_RAMP,
               .delay = ROAR_ORANGE_RING_DELAY});
}

}  // namespace rtype::client
