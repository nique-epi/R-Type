#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief What a ban names: a nickname, compared by network::nicknameKey()
 * compares them, or an IPv4 address, compared as text. */
enum class BanKind : std::uint8_t {
  Nickname,
  Address,
};

}  // namespace rtype::server
