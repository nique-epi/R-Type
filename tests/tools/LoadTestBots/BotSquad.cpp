#include "BotSquad.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <string>
#include <vector>
#include "Bot.hpp"
#include "BotConstants.hpp"
#include "BotLaunch.hpp"
#include "BotReport.hpp"
#include "ConnectionPhase.hpp"
#include "ReadyRule.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

namespace {

/** @returns The nickname of bot @p index, from 0: BOT_NICKNAME_PREFIX then
 * @p index + 1. */
std::string nicknameOf(std::size_t index) {
  return std::string{BOT_NICKNAME_PREFIX} + std::to_string(index + 1);
}

}  // namespace

BotSquad::BotSquad(const BotLaunch& launch) {
  for (std::size_t index = 0; index < launch.botCount; ++index) {
    Bot& bot =
        bots_.emplace_back(launch.seed + static_cast<std::uint32_t>(index));
    bot.join(launch.server, nicknameOf(index));
  }
}

void BotSquad::update(engine::Duration elapsed) {
  for (Bot& bot : bots_) {
    bot.update(elapsed);
  }
  const bool connecting = anyBotConnecting();
  for (Bot& bot : bots_) {
    if (asksReady(bot.standing(), connecting)) {
      bot.askReady();
    }
  }
}

std::vector<BotReport> BotSquad::report() const {
  std::vector<BotReport> reports;
  reports.reserve(bots_.size());
  for (const Bot& bot : bots_) {
    reports.push_back(bot.report());
  }
  return reports;
}

void BotSquad::leave() {
  std::exception_ptr firstFailure;
  for (Bot& bot : bots_) {
    try {
      bot.leave();
    } catch (const std::exception&) {
      if (!firstFailure) {
        firstFailure = std::current_exception();
      }
    }
  }
  if (firstFailure) {
    std::rethrow_exception(firstFailure);
  }
}

bool BotSquad::anyBotConnecting() const {
  return std::ranges::any_of(bots_, [](const Bot& bot) {
    return bot.standing().phase == client::ConnectionPhase::Connecting;
  });
}

}  // namespace rtype::tools
