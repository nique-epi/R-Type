#pragma once

#include <optional>

namespace rtype::client::testing {

/** @brief A share of a roll the prediction of the own ship wrote, and the
 * share a roll a world state just started reads: 0. */
constexpr float PREDICTED_ROLL_SHARE = 0.3F;
constexpr float START_OF_A_DRAWN_ROLL = 0.0F;

}  // namespace rtype::client::testing
