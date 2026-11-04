#include "ServerKeyText.hpp"
#include <hydrogen.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include "ServerKeysConstants.hpp"

namespace rtype::master {

namespace {

static_assert(FINGERPRINT_CONTEXT.size() == hydro_hash_CONTEXTBYTES);

std::string hexadecimalOf(std::span<const std::uint8_t> bytes) {
  std::string text((bytes.size() * HEX_DIGITS_PER_BYTE) + HEX_TERMINATOR_SIZE,
                   '\0');
  hydro_bin2hex(text.data(), text.size(), bytes.data(), bytes.size());
  text.resize(bytes.size() * HEX_DIGITS_PER_BYTE);
  return text;
}

}  // namespace

std::string newServerKeyText() {
  std::array<std::uint8_t, SERVER_KEY_BYTES> key{};
  hydro_random_buf(key.data(), key.size());
  return hexadecimalOf(key);
}

bool isWellFormedServerKey(std::string_view text) {
  return text.size() == SERVER_KEY_TEXT_SIZE &&
         std::ranges::all_of(text, [](char digit) {
           return std::ranges::find(SERVER_KEY_DIGITS, digit) !=
                  SERVER_KEY_DIGITS.end();
         });
}

std::string fingerprintOf(std::string_view keyText) {
  std::array<char, hydro_hash_CONTEXTBYTES> context{};
  std::ranges::copy(FINGERPRINT_CONTEXT, context.begin());
  std::array<std::uint8_t, hydro_hash_BYTES> fingerprint{};
  hydro_hash_hash(fingerprint.data(), fingerprint.size(), keyText.data(),
                  keyText.size(), context.data(), nullptr);
  return hexadecimalOf(fingerprint);
}

}  // namespace rtype::master
