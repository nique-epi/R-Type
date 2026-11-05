#pragma once

#include <cstddef>
#include <optional>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Decides when a game process must end by itself: its supervisor stopped
 * speaking (SUPERVISOR_SILENCE_LIMIT), the players it had are all gone for
 * EMPTY_GAME_GRACE, or nobody came for IDLE_GAME_TIMEOUT. A game that finished
 * a round is not a reason: it waits for players again, as a game always did.
 * Time passes only through tick(), so the decision needs no clock.
 */
class GameProcessWatch {
 public:
  /** @brief Lets @p elapsed pass, with @p playerCount players connected now. */
  void tick(engine::Duration elapsed, std::size_t playerCount);

  /** @brief Notes that the supervisor spoke just now. */
  void heardSupervisor();

  /** @returns Why the process must stop now, nothing while it must not. */
  [[nodiscard]] std::optional<std::string_view> reasonToStop() const;

 private:
  engine::Duration sinceSupervisor_{};
  engine::Duration withoutPlayers_{};
  bool hadPlayers_{false};
};

}  // namespace rtype::server
