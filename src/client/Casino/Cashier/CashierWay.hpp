#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Which way the cashier changes: kopecks into chips, or chips back
 * into kopecks. */
enum class CashierWay : std::uint8_t {
  BuyChips,
  CashOut,
};

}  // namespace rtype::client
