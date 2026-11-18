#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief What IAccountWallet::creditGame() did. */
enum class GameCreditOutcome : std::uint8_t {
  /** @brief The kopecks and the looks are in the wallet. */
  Credited,
  /** @brief The reference was credited before: nothing moved. */
  AlreadyCredited,
  /** @brief The master gave no ticket of that text, or it was used, or it
   * expired: nothing moved. */
  UnknownTicket,
  /** @brief The account the ticket was given to is gone: nothing moved. */
  UnknownAccount,
};

}  // namespace rtype::master
