#pragma once

#include <string>
#include <string_view>

namespace rtype::master {

/**
 * @returns A new server key: network::SERVER_KEY_PREFIX, then
 * network::SERVER_KEY_BYTES drawn by network::fillWithRandomBytes() in
 * lowercase hexadecimal.
 */
[[nodiscard]] std::string newServerKey();

/**
 * @returns The fingerprint of @p key kept in the database in place of the
 * key: its hash by network::hashOf() in FINGERPRINT_CONTEXT, in hexadecimal. A
 * key is 256 random bits, so a fast hash is enough: nobody can try every key,
 * unlike every password.
 */
[[nodiscard]] std::string fingerprintOf(std::string_view key);

}  // namespace rtype::master
