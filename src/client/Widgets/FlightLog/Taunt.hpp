#pragma once

#include <string_view>
#include "TauntCondition.hpp"

namespace rtype::client {

/** @brief A taunt of the cover: its lines, TAUNT_LINE_BREAK between them,
 * where BEST_PILOT_MARK, LAST_PILOT_MARK and ZERO_PILOT_MARK stand for a
 * name, and when it may show. */
struct Taunt {
  std::string_view text;
  TauntCondition condition{};
};

}  // namespace rtype::client
