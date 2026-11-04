#include "BotTally.hpp"
#include <variant>
#include "BotReport.hpp"
#include "EntityMessage.hpp"
#include "GameEnded.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::tools {

void BotTally::count(const client::EntityMessage& message) {
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

void BotTally::addTime(engine::Duration elapsed, bool gameStarted) {
  if (newestTick_.has_value() && gameStarted) {
    counts_.timeInGame += elapsed;
  }
}

void BotTally::countInput() {
  if (newestTick_.has_value()) {
    ++counts_.inputsInGame;
  }
}

BotReport BotTally::report() const { return counts_; }

}  // namespace rtype::tools
