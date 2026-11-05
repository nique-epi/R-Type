#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::master {

/** @brief The byte hydro_bin2hex() writes after the digits. */
constexpr std::size_t HEX_TERMINATOR_SIZE = 1;

/**
 * @brief The context of libhydrogen the fingerprint of a key is computed in:
 * exactly hydro_hash_CONTEXTBYTES characters, so no other hash of the master
 * can give the same value.
 */
constexpr std::string_view FINGERPRINT_CONTEXT = "srvkeys1";

}  // namespace rtype::master
