#include "KopecksReward.hpp"
#include <algorithm>
#include <cstdint>
#include <span>
#include "FinalPilot.hpp"
#include "GameEnded.hpp"
#include "Kopecks.hpp"
#include "PlayerSettings.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

std::uint32_t kopecksEarnedBy(std::span<const FinalPilot> pilots,
                              network::WireGameOutcome outcome) {
  const auto own = std::ranges::find_if(pilots, &FinalPilot::isLocalPlayer);
  if (own == pilots.end()) {
    return 0;
  }
  const std::uint32_t earned = kopecksFor(own->score);
  return outcome == network::WireGameOutcome::Victory ? withVictoryBonus(earned)
                                                      : earned;
}

void creditKopecks(SettingsService& settings, std::uint32_t gain) {
  if (gain == 0) {
    return;
  }
  const std::uint32_t balance =
      boundedKopecks(std::uint64_t{settings.settings().kopecks} + gain);
  static_cast<void>(settings.apply(KOPECKS_SETTING, balance));
  static_cast<void>(settings.saveIfChanged());
}

}  // namespace rtype::client
