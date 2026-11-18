#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "Wallet.hpp"

namespace rtype::master {

/** @brief A purchase of a look the shop accepted, about to be written. */
struct LookOrder {
  std::int64_t userId{};
  /** @brief Its position in identifiers::SHIP_LOOK_ITEM_NAMES. */
  std::size_t item{};
  /** @brief The balances before it. */
  network::Wallet before;
  std::chrono::sys_seconds boughtAt;
};

}  // namespace rtype::master
