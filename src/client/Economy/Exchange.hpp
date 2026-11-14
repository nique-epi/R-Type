#pragma once

#include <optional>
#include "ExchangeRefusal.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @brief What became of an exchange at the cashier: why it was refused, or
 * nothing once the amount is changed, and the purse after it, unchanged when
 * refused. */
struct Exchange {
  std::optional<ExchangeRefusal> refusal;
  Purse purse;
};

}  // namespace rtype::client
