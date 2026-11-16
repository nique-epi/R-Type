#pragma once

#include <cstdint>

namespace rtype::network {

/** @brief What a wallet of the master holds: written `kopecks` and `chips` in
 * JSON. Kopecks pay at the shop, chips at the casino; the cashier changes one
 * for the other, one for one. */
enum class Currency : std::uint8_t {
  Kopecks,
  Chips,
};

}  // namespace rtype::network
