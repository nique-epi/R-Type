#include "Hashing.hpp"
#include <hydrogen.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "HashingConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "PasswordHash.hpp"
#include "PasswordKey.hpp"
#include "SecretComparison.hpp"

namespace rtype::network {

static_assert(HASH_CONTEXT_LENGTH == hydro_hash_CONTEXTBYTES);
static_assert(HASH_BYTES >= hydro_hash_BYTES_MIN &&
              HASH_BYTES <= hydro_hash_BYTES_MAX);
static_assert(SECRET_COMPARISON_CONTEXT.size() == HASH_CONTEXT_LENGTH);
static_assert(PASSWORD_KEY_BYTES == hydro_pwhash_MASTERKEYBYTES);
static_assert(PASSWORD_HASH_BYTES == hydro_pwhash_STOREDBYTES);

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

std::optional<PasswordKey> passwordKeyOfHexadecimal(std::string_view text) {
  PasswordKey key{};
  const int written = hydro_hex2bin(key.data(), key.size(), text.data(),
                                    text.size(), nullptr, nullptr);
  if (std::cmp_not_equal(written, key.size())) {
    return std::nullopt;
  }
  return key;
}

PasswordHash passwordHashOf(std::string_view password, const PasswordKey& key) {
  PasswordHash hash{};
  if (hydro_pwhash_create(hash.data(), password.data(), password.size(),
                          key.data(), PASSWORD_HASH_ROUNDS,
                          PASSWORD_HASH_MEMORY, PASSWORD_HASH_THREADS) != 0) {
    throw PasswordHashingException();
  }
  return hash;
}

bool passwordMatches(std::string_view password, const PasswordHash& hash,
                     const PasswordKey& key) {
  return hydro_pwhash_verify(hash.data(), password.data(), password.size(),
                             key.data(), PASSWORD_HASH_ROUNDS,
                             PASSWORD_HASH_MEMORY, PASSWORD_HASH_THREADS) == 0;
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
