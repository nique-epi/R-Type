#pragma once

#include <cstdint>
#include <vector>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The numbers of the GameEvents seen in the last GAME_EVENT_MEMORY, so
 * the second and third copies of an event are told from the first.
 */
class GameEventHistory {
 public:
  /** @returns Whether an event of that number is the first copy seen in the
   * last GAME_EVENT_MEMORY; its number is remembered then. */
  [[nodiscard]] bool isFirstCopy(std::uint16_t eventNumber);

  /** @brief Ages the numbers remembered, and forgets those seen
   * GAME_EVENT_MEMORY ago or more. */
  void advance(engine::Duration elapsed);

  /** @brief Forgets every number: the events of a new session are numbered
   * from 1 again. */
  void forget();

 private:
  struct SeenEvent {
    std::uint16_t eventNumber{};
    engine::Duration age{};
  };

  std::vector<SeenEvent> seen_;
};

}  // namespace rtype::client
