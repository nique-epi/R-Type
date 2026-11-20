#pragma once

#include <string>

namespace rtype::client {

/** @brief What the cashier says in place of the result of the exchange: its
 * text, and whether it refuses, written in red, or tells what it does,
 * written in gold. */
struct CashierNotice {
  std::string text;
  bool refusal{};
};

}  // namespace rtype::client
