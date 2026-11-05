#pragma once

#include <cstdint>
#include <optional>
#include "BotReport.hpp"
#include "EntityMessage.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief What a bot counts of its run: the world states it received, each tick
 * once, the games it saw end, the time it spent in a game and the inputs it
 * sent then. For the tally, a game runs from its first world state to its
 * GameEnded, or to the end of the session.
 */
class BotTally {
 public:
  /**
   * @brief Counts @p message when it is a world state of a new tick or the
   * end of the game.
   *
   * Comparing with the newest tick counted is enough: the server sends every
   * part of a tick before the next tick, and the session drops a datagram that
   * is not newer than the newest one it accepted, so a part of an older tick
   * never arrives after a newer one. The tick is forgotten at the end of each
   * game, since the world of the next one counts from 0 again.
   */
  void count(const client::EntityMessage& message);

  /**
   * @brief Adds @p elapsed to the time in game while a game the tally counted
   * a world state of runs, and @p gameStarted says the session still has it.
   *
   * The frame that drains the first world state counts, the one that drains
   * the GameEnded does not. Each message arrived during the frame that drains
   * it, so the start of a game counts up to a frame too much and its end up to
   * a frame too little: they balance, where counting both frames would add a
   * frame to every game.
   */
  void addTime(engine::Duration elapsed, bool gameStarted);

  /** @brief Counts an input the bot sent, when it went out during a game. */
  void countInput();

  /** @returns What was counted; its phase stays Idle, for the bot to set. */
  [[nodiscard]] BotReport report() const;

 private:
  BotReport counts_;
  /** @brief The tick of the newest world state of the game in progress;
   * nothing before its first one, and once it ended. */
  std::optional<std::uint32_t> newestTick_;
};

}  // namespace rtype::tools
