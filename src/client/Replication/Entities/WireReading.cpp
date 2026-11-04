#include "WireReading.hpp"
#include <algorithm>
#include <optional>
#include "ActionKey.hpp"
#include "EnemyAction.hpp"
#include "EnemyPhase.hpp"
#include "WireReadingConstants.hpp"
#include "WireReadingRow.hpp"
#include "WorldState.hpp"

namespace rtype::client {

std::optional<AnnouncedEntity> announcedEntityOf(
    network::WireEntityType wireType) {
  const auto* const found =
      std::ranges::find(WIRE_READINGS, wireType, &WireReadingRow::wireType);
  if (found == WIRE_READINGS.end()) {
    return std::nullopt;
  }
  return found->entity;
}

game::ActionKey actionKeyOf(const network::EnemyAction& message) {
  switch (message.phase) {
    case network::EnemyActionPhase::Signal:
      return {.attack = message.attack, .phase = game::EnemyPhase::Signal};
    case network::EnemyActionPhase::Strike:
      return {.attack = message.attack, .phase = game::EnemyPhase::Strike};
    case network::EnemyActionPhase::Recover:
      return {.attack = message.attack, .phase = game::EnemyPhase::Recover};
    case network::EnemyActionPhase::Enraged:
      return {.attack = message.attack, .phase = game::EnemyPhase::Enraged};
  }
  return {.attack = message.attack, .phase = game::EnemyPhase::Signal};
}

}  // namespace rtype::client
