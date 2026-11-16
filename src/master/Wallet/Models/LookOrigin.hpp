#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief How an account came to own a look: written `starter`, `purchase`
 * and `game` in the table `owned_looks`. */
enum class LookOrigin : std::uint8_t {
  Starter,
  Purchase,
  Game,
};

}  // namespace rtype::master
