#pragma once

#include <array>
#include <cstdint>
#include "HashingConstants.hpp"

namespace rtype::network {

/**
 * @brief The key every password hash is encrypted with
 * (hydro_pwhash_MASTERKEYBYTES of libhydrogen): it lives outside the database,
 * so a stolen database lets nobody test a password.
 */
using PasswordKey = std::array<std::uint8_t, PASSWORD_KEY_BYTES>;

}  // namespace rtype::network
