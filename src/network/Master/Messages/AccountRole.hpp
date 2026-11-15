#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief What an account may do on the master: written `player` and `admin` in
 * JSON. Every account is a player when it is created; the `admin` command of
 * master.cfg makes one an admin.
 */
enum class AccountRole : std::uint8_t {
  Player,
  Admin,
};

}  // namespace rtype::network
