#pragma once

#include <deque>
#include <vector>
#include "Bot.hpp"
#include "BotLaunch.hpp"
#include "BotReport.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief The bots of one launch, moved together. A std::deque keeps each bot
 * in place while the next ones are built: a Bot holds a NetworkThread, which
 * can be neither copied nor moved.
 */
class BotSquad {
 public:
  /**
   * @brief Builds the botCount bots of @p launch, bot n (from 0) drawing from
   * the seed of @p launch plus n and named BOT_NICKNAME_PREFIX then n + 1,
   * opens their sockets and has each start joining the server of @p launch.
   * @throws SocketOpenException when the system refuses a socket.
   */
  explicit BotSquad(const BotLaunch& launch);

  /** @brief Moves every bot by @p elapsed, then has each bot that asksReady()
   * picks ask to be ready. */
  void update(engine::Duration elapsed);

  /** @returns The report of each bot, in the order they were built. */
  [[nodiscard]] std::vector<BotReport> report() const;

  /** @brief Has every bot leave its session, one after the other. */
  void leave();

 private:
  /** @returns Whether a bot still waits for the answer to its connection. */
  [[nodiscard]] bool anyBotConnecting() const;

  std::deque<Bot> bots_;
};

}  // namespace rtype::tools
