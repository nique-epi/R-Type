#pragma once

#include "TauntCondition.hpp"
#include "Translation.hpp"

namespace rtype::client {

/** @brief A taunt of the cover: its lines, TAUNT_LINE_BREAK between them,
 * where BEST_PILOT_MARK, LAST_PILOT_MARK and ZERO_PILOT_MARK stand for a
 * name, and when it may show. */
struct Taunt {
  Translation text;
  TauntCondition condition{};
};

}  // namespace rtype::client
