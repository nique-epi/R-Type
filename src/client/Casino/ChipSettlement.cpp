#include "ChipSettlement.hpp"
#include <algorithm>
#include <cstdint>
#include "ChipMoves.hpp"
#include "ICasinoChannel.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingsConstants.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

std::uint32_t chipsAfter(std::uint32_t chips, ChipMoves moves) {
  const std::uint64_t kept =
      chips > moves.staked ? std::uint64_t{chips} - moves.staked : 0;
  const std::uint64_t room = MAXIMUM_CHIPS > kept ? MAXIMUM_CHIPS - kept : 0;
  return static_cast<std::uint32_t>(
      std::min<std::uint64_t>(MAXIMUM_CHIPS, kept + std::min(moves.won, room)));
}

void settleChipMoves(ICasinoChannel& casino, SettingsService& settings) {
  const ChipMoves moves = casino.takeChipMoves();
  if (moves == ChipMoves{}) {
    return;
  }
  static_cast<void>(settings.apply(
      CHIPS_SETTING, chipsAfter(settings.settings().chips, moves)));
  static_cast<void>(settings.saveIfChanged());
}

}  // namespace rtype::client
