#pragma once

#include <optional>
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @brief What became of a purchase: why it was refused, or nothing once the
 * item is bought, and the purse after it, unchanged when refused. */
struct Purchase {
  std::optional<PurchaseRefusal> refusal;
  Purse purse;
};

}  // namespace rtype::client
