#pragma once

#include <string_view>

namespace rtype::master {

/** @brief The references the wallet gives the lines of the journal it writes
 * itself: the opening of a wallet, an exchange (followed by the number of the
 * line that took the amount, which both lines of the exchange name) and a
 * purchase (followed by the name of the look). */
constexpr std::string_view OPENING_REFERENCE = "opening";
constexpr std::string_view EXCHANGE_REFERENCE_PREFIX = "exchange:";
constexpr std::string_view LOOK_REFERENCE_PREFIX = "look:";

}  // namespace rtype::master
