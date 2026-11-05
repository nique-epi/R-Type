#pragma once

#include <string>
#include <string_view>

namespace rtype::master {

/**
 * @returns A new server key: network::SERVER_KEY_PREFIX, then
 * network::SERVER_KEY_BYTES drawn by hydro_random_buf() in lowercase
 * hexadecimal. libhydrogen seeds it from the system (getrandom-backed
 * `/dev/urandom` on Linux once `/dev/random` is ready, `/dev/urandom` on macOS,
 * RtlGenRandom on Windows).
 */
[[nodiscard]] std::string newServerKey();

/**
 * @returns The fingerprint of @p key kept in the database in place of the
 * key: its hash by libhydrogen (hydro_hash_hash(), in FINGERPRINT_CONTEXT, with
 * no key), in hexadecimal. A key is 256 random bits, so a fast hash is enough:
 * nobody can try every key, unlike every password.
 */
[[nodiscard]] std::string fingerprintOf(std::string_view key);

}  // namespace rtype::master
