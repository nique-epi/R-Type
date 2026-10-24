#pragma once

#include "FlashLoan.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

struct EffectAnimations;

/** @brief An effect of the sheet: its two versions, where it plays, how much
 * longer its first frame stays, and the pixels its flash takes for how
 * long. */
struct SheetEffect {
  const EffectAnimations* animations{};
  game::Position center{};
  engine::Duration hold{};
  FlashLoan flash;
};

}  // namespace rtype::client
