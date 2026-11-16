#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief Why the master did not make a purchase or an exchange:
 * BalanceRefused when the balances do not allow it (too few kopecks for a
 * price, too little of the currency changed, or the other balance full),
 * NotForSale for a look the shop does not sell, UnknownItem for a name no look
 * has, SignedOut when the session of the token ended, MasterRefused for any
 * other refusal or an answer the client cannot read, MasterUnreachable when no
 * master answered.
 */
enum class WalletRefusal : std::uint8_t {
  BalanceRefused,
  NotForSale,
  UnknownItem,
  SignedOut,
  MasterRefused,
  MasterUnreachable,
};

}  // namespace rtype::client
