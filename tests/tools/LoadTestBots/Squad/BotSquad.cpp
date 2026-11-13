#include "BotSquad.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <vector>
#include "Bot.hpp"
#include "BotLaunch.hpp"
#include "BotPilot.hpp"
#include "BotPilotKind.hpp"
#include "BotReport.hpp"
#include "BotSquadConstants.hpp"
#include "ConnectionPhase.hpp"
#include "IBotPilot.hpp"
#include "ReadyRule.hpp"
#include "RulePilot.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

namespace {

/** @returns The nickname of bot @p index, from 0: BOT_NICKNAME_PREFIX then
 * @p index + 1. */
std::string nicknameOf(std::size_t index) {
  return std::string{BOT_NICKNAME_PREFIX} + std::to_string(index + 1);
}

/** @returns The pilot of bot @p index of @p launch. */
std::unique_ptr<IBotPilot> pilotFor(const BotLaunch& launch,
                                    std::size_t index) {
  switch (launch.pilot) {
    case BotPilotKind::Rules:
      return std::make_unique<RulePilot>(index);
    case BotPilotKind::Random:
      break;
  }
  return std::make_unique<BotPilot>(launch.seed +
                                    static_cast<std::uint32_t>(index));
}

}  // namespace

BotSquad::BotSquad(const BotLaunch& launch) {
  for (std::size_t index = 0; index < launch.botCount; ++index) {
    Bot& bot = bots_.emplace_back(pilotFor(launch, index));
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
