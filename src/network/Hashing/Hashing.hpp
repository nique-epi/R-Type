#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include "HashingConstants.hpp"
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

/** @returns @p bytes in lowercase hexadecimal, HEX_DIGITS_PER_BYTE digits per
 * byte. */
[[nodiscard]] std::string hexadecimalOf(std::span<const std::byte> bytes);

}  // namespace rtype::network
