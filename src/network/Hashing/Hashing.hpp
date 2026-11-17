#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include "HashingConstants.hpp"
#include "PasswordHash.hpp"
#include "PasswordKey.hpp"
#include "SecretComparison.hpp"

namespace rtype::network {

/**
 * @returns The hash of @p bytes by libhydrogen (hydro_hash_hash(), without
 * key) in @p context, so two uses of the hash with two contexts never give
 * the same value for the same bytes.
 * @param context Exactly HASH_CONTEXT_LENGTH characters; its callers check it
 * with a static_assert, since it is always a constant.
 */
[[nodiscard]] std::array<std::byte, HASH_BYTES> hashOf(
    std::span<const std::byte> bytes, std::string_view context);

/**
 * @brief Fills @p bytes with random bytes drawn by hydro_random_buf(), which
 * libhydrogen seeds from the system (getrandom-backed `/dev/urandom` on Linux
 * once `/dev/random` is ready, `/dev/urandom` on macOS, RtlGenRandom on
 * Windows).
 */
void fillWithRandomBytes(std::span<std::byte> bytes);

/**
 * @returns Whether the presented secret of @p secrets is the expected one.
 * Both are hashed by hashOf() in SECRET_COMPARISON_CONTEXT, and the hashes
 * are compared by hydro_equal() of libhydrogen, whose time does not depend on
 * the bytes: the time of an answer tells a caller neither the length of the
 * expected secret nor how much of its guess was right.
 */
[[nodiscard]] bool secretsMatch(const SecretComparison& secrets);

/**
 * @returns The key @p text writes as PASSWORD_KEY_BYTES in hexadecimal, digits
 * in any case, as `openssl rand -hex 32` draws one; nothing for any other text,
 * one digit too few or too many included.
 */
[[nodiscard]] std::optional<PasswordKey> passwordKeyOfHexadecimal(
    std::string_view text);

/**
 * @returns What is kept of @p password: hydro_pwhash_create() of libhydrogen,
 * with a random salt and PASSWORD_HASH_ROUNDS, encrypted with @p key. Two
 * hashes of one password differ, since each has a salt of its own.
 * @throws PasswordHashingException when libhydrogen fails to hash it.
 */
[[nodiscard]] PasswordHash passwordHashOf(std::string_view password,
                                          const PasswordKey& key);

/**
 * @returns Whether @p hash was made of @p password with @p key, by
 * hydro_pwhash_verify(), which compares in a time that does not depend on the
 * bytes. False for another password, another key, or a hash made with more
 * than PASSWORD_HASH_ROUNDS.
 */
[[nodiscard]] bool passwordMatches(std::string_view password,
                                   const PasswordHash& hash,
                                   const PasswordKey& key);

/** @returns @p bytes in lowercase hexadecimal, HEX_DIGITS_PER_BYTE digits per
 * byte. */
[[nodiscard]] std::string hexadecimalOf(std::span<const std::byte> bytes);

}  // namespace rtype::network
