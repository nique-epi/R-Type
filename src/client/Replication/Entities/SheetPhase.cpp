#include "SheetPhase.hpp"
#include "EnemyAction.hpp"
#include "EnemyPhase.hpp"

namespace rtype::client {

game::EnemyPhase sheetPhaseOf(network::EnemyActionPhase phase) {
  switch (phase) {
    case network::EnemyActionPhase::Signal:
      return game::EnemyPhase::Signal;
    case network::EnemyActionPhase::Strike:
      return game::EnemyPhase::Strike;
    case network::EnemyActionPhase::Recover:
      return game::EnemyPhase::Recover;
    case network::EnemyActionPhase::Enraged:
      return game::EnemyPhase::Enraged;
  }
  return game::EnemyPhase::Signal;
}

}  // namespace rtype::client
