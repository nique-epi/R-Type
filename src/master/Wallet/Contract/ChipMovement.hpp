#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include "MovementReason.hpp"

namespace rtype::master {

/** @brief A change of the chips of a wallet another module asks for, such as
 * a stake, a refund or a payout of the casino. */
struct ChipMovement {
  /** @brief The account whose wallet moves; opened first when it never was. */
  std::int64_t userId{};
  /**
   * @brief What the chips gain, negative for what they lose. A loss larger
   * than the balance is refused; a gain past network::MAXIMUM_CHIPS stops at
   * it, and the journal keeps what was really added.
   */
  std::int64_t amount{};
  network::MovementReason reason{network::MovementReason::Stake};
  /** @brief The fact that moves the chips, kept in the journal: such as
   * `round:<room>:<round>:<user>`. */
  std::string reference;
  std::chrono::sys_seconds madeAt;
};

}  // namespace rtype::master
