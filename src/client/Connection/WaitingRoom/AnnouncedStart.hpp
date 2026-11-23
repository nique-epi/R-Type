#pragma once

#include <optional>
#include "GameStarted.hpp"

namespace rtype::client {

/**
 * @brief What the server told of the level as the game of the session
 * started: the GameStarted the waiting room received, so the game shows the
 * sky its level opens in and plays its music while it loads, before the
 * first LevelStatus.
 *
 * The waiting room holds each GameStarted it receives in a session, and
 * forgets it when the game ends or the session closes, so a game started by
 * a WorldState, its GameStarted lost, never shows the sky of the game before.
 * Not thread-safe: use it from the thread that updates the session.
 */
class AnnouncedStart {
 public:
  /** @brief Keeps @p started as the start of the game in progress. */
  void hold(network::GameStarted started);

  /** @brief Forgets the start of the game: no game is in progress. */
  void forget();

  /** @returns The GameStarted of the game in progress; nothing before one
   * arrived, and once the game ended. */
  [[nodiscard]] const std::optional<network::GameStarted>& started() const;

 private:
  std::optional<network::GameStarted> started_;
};

}  // namespace rtype::client
