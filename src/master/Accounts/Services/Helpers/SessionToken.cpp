#include "SessionToken.hpp"
#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include "Hashing.hpp"
#include "HashingConstants.hpp"
#include "MasterMessageConstants.hpp"
#include "SessionTokenConstants.hpp"

namespace rtype::master {

static_assert(SESSION_FINGERPRINT_CONTEXT.size() ==
              network::HASH_CONTEXT_LENGTH);

std::string newSessionToken() {
  std::array<std::byte, network::SESSION_TOKEN_BYTES> token{};
  network::fillWithRandomBytes(token);
  return std::string{network::SESSION_TOKEN_PREFIX} +
         network::hexadecimalOf(token);
}

std::string sessionFingerprintOf(std::string_view token) {
  return network::hexadecimalOf(
      network::hashOf(std::as_bytes(std::span{token.data(), token.size()}),
                      SESSION_FINGERPRINT_CONTEXT));
}

}  // namespace rtype::master
