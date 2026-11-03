#include "Bot.hpp"
#include <cstdint>
#include <string_view>
#include <tuple>
#include <variant>
#include "ActionSet.hpp"
#include "BotReport.hpp"
#include "BotStanding.hpp"
#include "Endpoint.hpp"
#include "EntityMessage.hpp"
#include "GameEnded.hpp"
#include "NetworkConditionsConstants.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::tools {

Bot::Bot(std::uint32_t seed)
    : connection_(network::PERFECT_NETWORK), pilot_(seed) {}

void Bot::join(const network::Endpoint& server, std::string_view nickname) {
  connection_.network().open();
  connection_.session().connect(server, nickname);
}

void Bot::update(engine::Duration elapsed) {
  connection_.update(elapsed);
  countReceived(elapsed);
  play(elapsed);
}

void Bot::askReady() { connection_.waitingRoom().setReady(true); }

BotStanding Bot::standing() const {
  return BotStanding{
      .phase = connection_.session().status().phase,
      .gameStarted = connection_.waitingRoom().gameStarted(),
      .requestedReady = connection_.waitingRoom().requestedReady()};
}

BotReport Bot::report() const {
  BotReport report = counts_;
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
    count(message);
  }
  if (newestTick_.has_value() && connection_.waitingRoom().gameStarted()) {
    counts_.timeInGame += elapsed;
  }
}

void Bot::count(const client::EntityMessage& message) {
  if (const auto* state = std::get_if<network::WorldState>(&message)) {
    if (state->tick != newestTick_) {
      ++counts_.worldStates;
      newestTick_ = state->tick;
    }
  } else if (std::holds_alternative<network::GameEnded>(message)) {
    ++counts_.gamesEnded;
    newestTick_.reset();
  }
}

void Bot::play(engine::Duration elapsed) {
  if (!connection_.waitingRoom().gameStarted()) {
    return;
  }
  const client::ActionSet held = pilot_.advance(elapsed);
  if (pacer_.advance(elapsed)) {
    std::ignore = connection_.session().sendPlayerInput(held);
  }
}

}  // namespace rtype::tools
