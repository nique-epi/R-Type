#include "RulesRefusal.hpp"
#include <optional>
#include "ConnectionRefused.hpp"
#include "GameRuleConstants.hpp"
#include "GameRules.hpp"
#include "HostedDifficulties.hpp"

namespace rtype::server {

std::optional<network::RefusalReason> refusalOf(
    const network::GameRules& rules, const HostedDifficulties& difficulties) {
  if (!difficulties.contains(rules.difficulty)) {
    return network::RefusalReason::UnknownDifficulty;
  }
  if (rules.lives < identifiers::LOWEST_LIVES ||
      rules.lives > identifiers::HIGHEST_LIVES) {
    return network::RefusalReason::LivesOutOfRange;
  }
  return std::nullopt;
}

}  // namespace rtype::server
