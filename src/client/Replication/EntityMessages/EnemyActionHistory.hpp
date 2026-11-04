#pragma once

#include <vector>
#include "EnemyAction.hpp"
#include "SeenEnemyAction.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The EnemyActions seen in the last GAME_EVENT_MEMORY, so a copy the
 * server sends again, when the acknowledgement of the first was lost, is told
 * from the first. Two messages with the same entity, attack and phase are
 * copies: a behaviour does not enter one phase of one attack twice within that
 * time.
 */
class EnemyActionHistory {
 public:
  /** @returns Whether no message equal to @p action was seen in the last
   * GAME_EVENT_MEMORY; it is remembered then. */
  [[nodiscard]] bool isFirstCopy(const network::EnemyAction& action);

  /** @brief Ages the messages remembered, and forgets those seen
   * GAME_EVENT_MEMORY ago or more. */
  void advance(engine::Duration elapsed);

  /** @brief Forgets every message. */
  void forget();

 private:
  std::vector<SeenEnemyAction> seen_;
};

}  // namespace rtype::client
