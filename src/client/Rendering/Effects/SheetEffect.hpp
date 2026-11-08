#pragma once

#include <string_view>
#include "EffectConstants.hpp"
#include "FlashLoan.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

struct EffectAnimations;

/** @brief An effect of a sheet: its two versions, where it plays, how much
 * longer its first frame stays, the pixels its flash takes for how long, and
 * the sheet its frames lie in, the effect sheet unless an entity sheet draws
 * the effect, as the death of an enemy. */
struct SheetEffect {
  const EffectAnimations* animations{};
  game::Position center{};
  engine::Duration hold{};
  FlashLoan flash;
  std::string_view sheet{EFFECT_SHEET_ID};
};

}  // namespace rtype::client
