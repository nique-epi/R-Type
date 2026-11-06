#include "BotTally.hpp"
#include <variant>
#include "BotReport.hpp"
#include "BotTraffic.hpp"
#include "EntityMessage.hpp"
#include "GameEnded.hpp"
#include "TimeConstants.hpp"
#include "TrafficTotals.hpp"
#include "WorldState.hpp"

namespace rtype::tools {

void BotTally::count(const client::EntityMessage& message) {
  if (const auto* state = std::get_if<network::WorldState>(&message)) {
    if (state->tick != newestTick_) {
      ++counts_.worldStates;
      counts_.entitiesListed += state->entities.size();
      newestTick_ = state->tick;
    }
  } else if (std::holds_alternative<network::GameEnded>(message)) {
    ++counts_.gamesEnded;
    newestTick_.reset();
  }
}

void BotTally::addTime(engine::Duration elapsed, bool gameStarted) {
  if (isInGame(gameStarted)) {
    counts_.timeInGame += elapsed;
  }
}

void BotTally::addTraffic(const BotTraffic& totals, bool gameStarted) {
  if (isInGame(gameStarted)) {
    network::addTotals(
        counts_.trafficInGame.plain,
        network::trafficBetween(previousTraffic_.plain, totals.plain));
    network::addTotals(
        counts_.trafficInGame.wire,
        network::trafficBetween(previousTraffic_.wire, totals.wire));
  }
  previousTraffic_ = totals;
}

void BotTally::countInput() {
  if (newestTick_.has_value()) {
    ++counts_.inputsInGame;
  }
}

BotReport BotTally::report() const { return counts_; }

bool BotTally::isInGame(bool gameStarted) const {
  return newestTick_.has_value() && gameStarted;
}

}  // namespace rtype::tools
