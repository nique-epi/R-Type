#pragma once

#include "BossGaugeState.hpp"

namespace rtype::client {

/** @brief One gauge: the columns its fill lights from the left, the column
 * its lag ends at (never before the fill), and its state. */
struct BossGaugeLook {
  int fill{};
  int lagEnd{};
  BossGaugeState state{BossGaugeState::Waiting};

  bool operator==(const BossGaugeLook&) const = default;
};

}  // namespace rtype::client
