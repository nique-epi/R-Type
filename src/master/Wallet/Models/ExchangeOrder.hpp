#pragma once

#include <chrono>
#include <cstdint>
#include "ExchangeRequest.hpp"
#include "Wallet.hpp"

namespace rtype::master {

/** @brief An exchange the cashier accepted, about to be written. */
struct ExchangeOrder {
  std::int64_t userId{};
  /** @brief The balances before it. */
  network::Wallet before;
  network::ExchangeRequest request;
  std::chrono::sys_seconds madeAt;
};

}  // namespace rtype::master
