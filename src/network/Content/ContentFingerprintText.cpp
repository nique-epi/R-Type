#include "ContentFingerprintText.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "ContentFingerprint.hpp"
#include "ContentPackConstants.hpp"
#include "Hashing.hpp"
#include "NetworkConstants.hpp"

namespace rtype::network {

namespace {

/** @returns The value of the lowercase hexadecimal digit @p digit, or nothing
 * for any other character. */
std::optional<std::uint8_t> digitValueOf(char digit) {
  const std::size_t value = LOWERCASE_HEX_DIGITS.find(digit);
  if (value == std::string_view::npos) {
    return std::nullopt;
  }
  return static_cast<std::uint8_t>(value);
}

}  // namespace

std::string fingerprintText(const ContentFingerprint& fingerprint) {
  return hexadecimalOf(fingerprint.bytes);
}

std::optional<ContentFingerprint> fingerprintFromText(std::string_view text) {
  if (text.size() != CONTENT_FINGERPRINT_TEXT_LENGTH) {
    return std::nullopt;
  }
  ContentFingerprint fingerprint;
  for (std::size_t index = 0; index < fingerprint.bytes.size(); ++index) {
    const std::optional<std::uint8_t> high =
        digitValueOf(text[index * HEX_DIGITS_PER_BYTE]);
    const std::optional<std::uint8_t> low =
        digitValueOf(text[(index * HEX_DIGITS_PER_BYTE) + 1]);
    if (!high.has_value() || !low.has_value()) {
      return std::nullopt;
    }
    fingerprint.bytes[index] =
        static_cast<std::byte>((*high * LOWERCASE_HEX_DIGITS.size()) + *low);
  }
  return fingerprint;
}

}  // namespace rtype::network
