#include "Bot.hpp"
#include <cstdint>
#include <string_view>
#include "BotReport.hpp"
#include "BotStanding.hpp"
#include "Endpoint.hpp"
#include "EntityMessage.hpp"
#include "NetworkConditionsConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

Bot::Bot(std::uint32_t seed)
    : connection_(network::PERFECT_NETWORK),
      inputs_(seed, connection_.waitingRoom(), connection_.session()) {}

void Bot::join(const network::Endpoint& server, std::string_view nickname) {
  connection_.network().open();
  connection_.session().connect(server, nickname);
}

void Bot::update(engine::Duration elapsed) {
  connection_.update(elapsed);
  countReceived(elapsed);
  if (inputs_.advance(elapsed)) {
    tally_.countInput();
  }
}

void Bot::askReady() { connection_.waitingRoom().setReady(true); }

BotStanding Bot::standing() const {
  return BotStanding{
      .phase = connection_.session().status().phase,
      .gameStarted = connection_.waitingRoom().gameStarted(),
      .requestedReady = connection_.waitingRoom().requestedReady()};
}

BotReport Bot::report() const {
  BotReport report = tally_.report();
  report.phase = connection_.session().status().phase;
  return report;
}

void Bot::leave() {
  connection_.leave();
  connection_.network().close();
}

void Bot::countReceived(engine::Duration elapsed) {
  received_.clear();
  connection_.entityMessages().drainEntityMessages(received_);
  for (const client::EntityMessage& message : received_) {
    tally_.count(message);
  }
  tally_.addTime(elapsed, connection_.waitingRoom().gameStarted());
}

}  // namespace rtype::tools
