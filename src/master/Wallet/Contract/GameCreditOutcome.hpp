#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief What IAccountWallet::creditGame() did. */
enum class GameCreditOutcome : std::uint8_t {
  /** @brief The kopecks and the looks are in the wallet. */
  Credited,
  /** @brief The reference was credited before: nothing moved. */
  AlreadyCredited,
  /** @brief No account has the user id: nothing moved. */
  UnknownAccount,
};

}  // namespace rtype::master
