#include "RulesRefusal.hpp"
#include <optional>
#include "ConnectionRefused.hpp"
#include "GameRules.hpp"
#include "HostedDifficulties.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

std::optional<network::RefusalReason> refusalOf(
    const network::GameRules& rules, const HostedDifficulties& difficulties) {
  if (!difficulties.contains(rules.difficulty)) {
    return network::RefusalReason::UnknownDifficulty;
  }
  if (rules.lives < LOWEST_LIVES || rules.lives > HIGHEST_LIVES) {
    return network::RefusalReason::LivesOutOfRange;
  }
  return std::nullopt;
}

}  // namespace rtype::server
