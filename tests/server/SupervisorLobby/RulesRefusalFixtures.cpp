#include "RulesRefusalFixtures.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "GameRules.hpp"
#include "HostedDifficulties.hpp"
#include "SupervisedGamesTestConstants.hpp"

namespace rtype::server::testing {

HostedDifficulties shippedDifficulties() {
  HostedDifficulties difficulties;
  difficulties.keep(std::vector<std::string>{SHIPPED_DIFFICULTIES.begin(),
                                             SHIPPED_DIFFICULTIES.end()});
  return difficulties;
}

network::GameRules rulesWithLives(std::uint8_t lives) {
  network::GameRules rules = network::defaultGameRules();
  rules.lives = lives;
  return rules;
}

}  // namespace rtype::server::testing
