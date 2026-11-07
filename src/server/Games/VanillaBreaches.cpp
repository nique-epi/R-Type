#include "VanillaBreaches.hpp"
#include <string_view>
#include <vector>
#include "HostedRules.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

std::vector<std::string_view> vanillaBreaches(const HostedRules& rules) {
  std::vector<std::string_view> breaches;
  if (rules.wavesFingerprint != OFFICIAL_WAVES_FINGERPRINT) {
    breaches.push_back(MODIFIED_WAVES_BREACH);
  }
  if (rules.sheetsFingerprint != OFFICIAL_ENTITY_SHEETS_FINGERPRINT) {
    breaches.push_back(MODIFIED_ENTITY_SHEETS_BREACH);
  }
  if (rules.seed.has_value()) {
    breaches.push_back(IMPOSED_SEED_BREACH);
  }
  if (rules.inputDelayTicks != DEFAULT_INPUT_DELAY_TICKS) {
    breaches.push_back(CHANGED_INPUT_DELAY_BREACH);
  }
  if (rules.startsAtBoss) {
    breaches.push_back(STARTS_AT_BOSS_BREACH);
  }
  return breaches;
}

}  // namespace rtype::server
