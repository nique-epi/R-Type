#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief Why a ChipMovement was not applied. */
enum class ChipRefusal : std::uint8_t {
  /** @brief It takes more chips than the wallet holds. */
  NotEnoughChips,
  /** @brief It applies once, and the journal holds its reference already. */
  ReferenceUsed,
  /** @brief No account has its user id. */
  UnknownAccount,
};

}  // namespace rtype::master
