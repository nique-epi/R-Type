#pragma once

#include <string>
#include <string_view>

namespace rtype::master {

/**
 * @returns A new server key: SERVER_KEY_BYTES drawn by hydro_random_buf(),
 * which libhydrogen seeds from the system (getrandom-backed `/dev/urandom` on
 * Linux once `/dev/random` is ready, `/dev/urandom` on macOS, RtlGenRandom on
 * Windows), written as SERVER_KEY_TEXT_SIZE lowercase hexadecimal digits.
 */
[[nodiscard]] std::string newServerKeyText();

/**
 * @returns Whether @p text could be a key the master issued: exactly
 * SERVER_KEY_TEXT_SIZE of SERVER_KEY_DIGITS. Anything else is refused before
 * its fingerprint is computed.
 */
[[nodiscard]] bool isWellFormedServerKey(std::string_view text);

/**
 * @returns The fingerprint of @p keyText kept in the database in place of the
 * key: its hash by libhydrogen (hydro_hash_hash(), in FINGERPRINT_CONTEXT, with
 * no key), in hexadecimal. A key is 256 random bits, so a fast hash is enough:
 * nobody can try every key, unlike every password.
 */
[[nodiscard]] std::string fingerprintOf(std::string_view keyText);

}  // namespace rtype::master
