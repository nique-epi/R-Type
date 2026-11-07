#include "ServerKey.hpp"
#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include "Hashing.hpp"
#include "HashingConstants.hpp"
#include "MasterMessageConstants.hpp"
#include "ServerKeyConstants.hpp"

namespace rtype::master {

static_assert(FINGERPRINT_CONTEXT.size() == network::HASH_CONTEXT_LENGTH);

std::string newServerKey() {
  std::array<std::byte, network::SERVER_KEY_BYTES> key{};
  network::fillWithRandomBytes(key);
  return std::string{network::SERVER_KEY_PREFIX} + network::hexadecimalOf(key);
}

std::string fingerprintOf(std::string_view key) {
  return network::hexadecimalOf(network::hashOf(
      std::as_bytes(std::span{key.data(), key.size()}), FINGERPRINT_CONTEXT));
}

}  // namespace rtype::master
