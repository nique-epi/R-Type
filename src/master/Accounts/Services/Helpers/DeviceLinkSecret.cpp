#include "DeviceLinkSecret.hpp"
#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include "DeviceLinkSecretConstants.hpp"
#include "Hashing.hpp"
#include "HashingConstants.hpp"
#include "MasterMessageConstants.hpp"

namespace rtype::master {

namespace {

/** @returns A letter of network::DEVICE_LINK_USER_CODE_ALPHABET, each as
 * likely as the others. */
char unbiasedLetter() {
  std::array<std::byte, 1> drawn{};
  for (;;) {
    network::fillWithRandomBytes(drawn);
    const auto value = std::to_integer<std::size_t>(drawn.front());
    if (value < USER_CODE_UNBIASED_LIMIT) {
      return network::DEVICE_LINK_USER_CODE_ALPHABET.at(
          value % network::DEVICE_LINK_USER_CODE_ALPHABET.size());
    }
  }
}

}  // namespace

static_assert(DEVICE_LINK_FINGERPRINT_CONTEXT.size() ==
              network::HASH_CONTEXT_LENGTH);

std::string newDeviceLinkSecret() {
  std::array<std::byte, network::DEVICE_LINK_SECRET_BYTES> secret{};
  network::fillWithRandomBytes(secret);
  return std::string{network::DEVICE_LINK_SECRET_PREFIX} +
         network::hexadecimalOf(secret);
}

std::string deviceLinkFingerprintOf(std::string_view secret) {
  return network::hexadecimalOf(
      network::hashOf(std::as_bytes(std::span{secret.data(), secret.size()}),
                      DEVICE_LINK_FINGERPRINT_CONTEXT));
}

std::string newDeviceLinkUserCode() {
  std::string code;
  for (std::size_t letter = 0; letter < network::DEVICE_LINK_USER_CODE_LETTERS;
       ++letter) {
    if (letter == network::DEVICE_LINK_USER_CODE_GROUP) {
      code.push_back(network::DEVICE_LINK_USER_CODE_SEPARATOR);
    }
    code.push_back(unbiasedLetter());
  }
  return code;
}

}  // namespace rtype::master
