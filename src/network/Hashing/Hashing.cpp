#include "Hashing.hpp"
#include <hydrogen.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include "HashingConstants.hpp"
#include "NetworkConstants.hpp"
#include "SecretComparison.hpp"

namespace rtype::network {

static_assert(HASH_CONTEXT_LENGTH == hydro_hash_CONTEXTBYTES);
static_assert(HASH_BYTES >= hydro_hash_BYTES_MIN &&
              HASH_BYTES <= hydro_hash_BYTES_MAX);
static_assert(SECRET_COMPARISON_CONTEXT.size() == HASH_CONTEXT_LENGTH);

namespace {

/** @returns The hash of @p secret in SECRET_COMPARISON_CONTEXT. */
std::array<std::byte, HASH_BYTES> secretHashOf(std::string_view secret) {
  return hashOf(std::as_bytes(std::span{secret.data(), secret.size()}),
                SECRET_COMPARISON_CONTEXT);
}

}  // namespace

std::array<std::byte, HASH_BYTES> hashOf(std::span<const std::byte> bytes,
                                         std::string_view context) {
  std::array<char, hydro_hash_CONTEXTBYTES> contextCharacters{};
  std::ranges::copy(context.substr(0, contextCharacters.size()),
                    contextCharacters.begin());
  std::array<std::uint8_t, HASH_BYTES> digest{};
  hydro_hash_hash(digest.data(), digest.size(), bytes.data(), bytes.size(),
                  contextCharacters.data(), nullptr);
  std::array<std::byte, HASH_BYTES> hash{};
  std::ranges::transform(digest, hash.begin(),
                         [](std::uint8_t value) { return std::byte{value}; });
  return hash;
}

void fillWithRandomBytes(std::span<std::byte> bytes) {
  hydro_random_buf(bytes.data(), bytes.size());
}

bool secretsMatch(const SecretComparison& secrets) {
  const std::array<std::byte, HASH_BYTES> presented =
      secretHashOf(secrets.presented);
  const std::array<std::byte, HASH_BYTES> expected =
      secretHashOf(secrets.expected);
  return hydro_equal(presented.data(), expected.data(), HASH_BYTES);
}

std::string hexadecimalOf(std::span<const std::byte> bytes) {
  std::string text;
  text.reserve(bytes.size() * HEX_DIGITS_PER_BYTE);
  for (const std::byte value : bytes) {
    const auto whole = std::to_integer<std::size_t>(value);
    text.push_back(LOWERCASE_HEX_DIGITS[whole / LOWERCASE_HEX_DIGITS.size()]);
    text.push_back(LOWERCASE_HEX_DIGITS[whole % LOWERCASE_HEX_DIGITS.size()]);
  }
  return text;
}

}  // namespace rtype::network
