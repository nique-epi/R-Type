#include "GameRequest.hpp"
#include <optional>
#include "ConnectionRequest.hpp"
#include "GameRules.hpp"

namespace rtype::client {

GameRequest gameRequestFor(const network::GameChoice& game) {
  if (game.kind == network::GameChoiceKind::ExistingGame) {
    return GameRequest{.game = game, .rules = std::nullopt};
  }
  return GameRequest{.game = game, .rules = network::defaultGameRules()};
}

std::optional<network::GameRules> rulesSentFor(const GameRequest& request) {
  if (request.game.kind == network::GameChoiceKind::NewGame &&
      request.rules.has_value()) {
    return request.rules;
  }
  return network::defaultRulesFor(request.game);
}

}  // namespace rtype::client
