#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief When a taunt may show: always, with several pilots, when the best
 * scored more than the last, when a pilot scored 0, or alone. */
enum class TauntCondition : std::uint8_t {
  Always,
  SeveralPilots,
  BestAheadOfLast,
  ZeroScore,
  Solo,
};

}  // namespace rtype::client
