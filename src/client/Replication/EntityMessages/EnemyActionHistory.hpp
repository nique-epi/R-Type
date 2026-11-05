#pragma once

#include <vector>
#include "EnemyAction.hpp"
#include "SeenEnemyAction.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The (entity, attack, phase) of the EnemyActions seen in the last
 * GAME_EVENT_MEMORY, so a copy the server sends again, when the acknowledgement
 * of the first was lost, is told from the first.
 *
 * The server never starts the same phase of the same attack of an enemy twice
 * within GAME_EVENT_MEMORY, so a second message with the same key within that
 * time is a copy.
 */
class EnemyActionHistory {
 public:
  /** @returns Whether @p action is the first copy of its (entity, attack,
   * phase) seen in the last GAME_EVENT_MEMORY; it is remembered then. */
  [[nodiscard]] bool isFirstCopy(const network::EnemyAction& action);

  /** @brief Ages what is remembered, and forgets what was seen
   * GAME_EVENT_MEMORY ago or more. */
  void advance(engine::Duration elapsed);

  /** @brief Forgets everything: the entities of a new session are numbered
   * from the start again. */
  void forget();

 private:
  std::vector<SeenEnemyAction> seen_;
};

}  // namespace rtype::client
