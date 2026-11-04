#include "EnemyActionHistory.hpp"
#include <algorithm>
#include <vector>
#include "EnemyAction.hpp"
#include "EntityMessageConstants.hpp"
#include "SeenEnemyAction.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

bool EnemyActionHistory::isFirstCopy(const network::EnemyAction& action) {
  if (std::ranges::any_of(seen_, [&action](const SeenEnemyAction& seen) {
        return seen.action == action;
      })) {
    return false;
  }
  seen_.push_back(
      SeenEnemyAction{.action = action, .age = engine::Duration::zero()});
  return true;
}

void EnemyActionHistory::advance(engine::Duration elapsed) {
  for (SeenEnemyAction& seen : seen_) {
    seen.age += elapsed;
  }
  std::erase_if(seen_, [](const SeenEnemyAction& seen) {
    return seen.age >= GAME_EVENT_MEMORY;
  });
}

void EnemyActionHistory::forget() { seen_.clear(); }

}  // namespace rtype::client
