#pragma once

#include <array>
#include <cstdint>
#include "HashingConstants.hpp"

namespace rtype::network {

/**
 * @brief What is kept of a password (hydro_pwhash_STOREDBYTES of libhydrogen):
 * its hash, its salt and the parameters of the hash, encrypted and
 * authenticated with a PasswordKey.
 */
using PasswordHash = std::array<std::uint8_t, PASSWORD_HASH_BYTES>;

}  // namespace rtype::network
