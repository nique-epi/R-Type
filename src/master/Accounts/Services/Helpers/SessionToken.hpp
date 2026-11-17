#pragma once

#include <string>
#include <string_view>

namespace rtype::master {

/**
 * @returns A new token of a session: network::SESSION_TOKEN_PREFIX, then
 * network::SESSION_TOKEN_BYTES drawn by network::fillWithRandomBytes() in
 * lowercase hexadecimal.
 */
[[nodiscard]] std::string newSessionToken();

/**
 * @returns The fingerprint of @p token kept in the database in place of the
 * token: its hash by network::hashOf() in SESSION_FINGERPRINT_CONTEXT, in
 * hexadecimal. A token is 256 random bits, so a fast hash is enough, as for
 * the keys of the servers.
 */
[[nodiscard]] std::string sessionFingerprintOf(std::string_view token);

}  // namespace rtype::master
