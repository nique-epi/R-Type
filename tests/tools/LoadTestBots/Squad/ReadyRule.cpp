#include "ReadyRule.hpp"
#include "BotStanding.hpp"
#include "ConnectionPhase.hpp"

namespace rtype::tools {

bool asksReady(const BotStanding& bot, bool anyBotConnecting) {
  return !anyBotConnecting && bot.phase == client::ConnectionPhase::Accepted &&
         !bot.gameStarted && !bot.requestedReady.has_value();
}

}  // namespace rtype::tools
