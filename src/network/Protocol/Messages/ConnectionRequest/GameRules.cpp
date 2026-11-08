#include "GameRules.hpp"
#include <string>
#include "GameRuleConstants.hpp"

namespace rtype::network {

GameRules defaultGameRules() {
  return GameRules{.difficulty = std::string{identifiers::DEFAULT_DIFFICULTY},
                   .lives = identifiers::DEFAULT_LIVES,
                   .friendlyFire = false};
}

}  // namespace rtype::network
