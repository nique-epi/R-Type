#include "ServerKey.hpp"
#include <hydrogen.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include "MasterMessageConstants.hpp"
#include "ServerKeyConstants.hpp"

namespace rtype::master {

namespace {

static_assert(FINGERPRINT_CONTEXT.size() == hydro_hash_CONTEXTBYTES);

std::string hexadecimalOf(std::span<const std::uint8_t> bytes) {
  const std::size_t digits = bytes.size() * network::HEX_DIGITS_PER_BYTE;
  std::string text(digits + HEX_TERMINATOR_SIZE, '\0');
  hydro_bin2hex(text.data(), text.size(), bytes.data(), bytes.size());
  text.resize(digits);
  return text;
}

}  // namespace

std::string newServerKey() {
  std::array<std::uint8_t, network::SERVER_KEY_BYTES> key{};
  hydro_random_buf(key.data(), key.size());
  return std::string{network::SERVER_KEY_PREFIX} + hexadecimalOf(key);
}

std::string fingerprintOf(std::string_view key) {
  std::array<char, hydro_hash_CONTEXTBYTES> context{};
  std::ranges::copy(FINGERPRINT_CONTEXT, context.begin());
  std::array<std::uint8_t, hydro_hash_BYTES> fingerprint{};
  hydro_hash_hash(fingerprint.data(), fingerprint.size(), key.data(),
                  key.size(), context.data(), nullptr);
  return hexadecimalOf(fingerprint);
}

}  // namespace rtype::master
