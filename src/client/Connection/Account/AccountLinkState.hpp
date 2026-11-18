#pragma once

#include "AccountAsks.hpp"
#include "LatestValue.hpp"
#include "UserReading.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"

namespace rtype::client {

/** @brief What the frame loop and the thread of a MasterAccountLink share:
 * the asks, newest first, and what the thread read, newest first; and the
 * asks it answered so far, which only that thread reads and writes. */
struct AccountLinkState {
  concurrency::LatestValue<AccountAsks> asks;
  AccountAsks served;
  concurrency::LatestValue<UserReading> user;
  concurrency::LatestValue<WalletReading> wallet;
  concurrency::LatestValue<WalletChangeReading> purchase;
  concurrency::LatestValue<WalletChangeReading> exchange;
};

}  // namespace rtype::client
