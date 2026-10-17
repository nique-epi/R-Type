#include "CookieIssuer.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include "ByteWriter.hpp"
#include "Endpoint.hpp"
#include "HandshakeConstants.hpp"
#include "SerializationConstants.hpp"
#include "SipHash.hpp"

namespace rtype::network {

namespace {

SipHashKey randomKey() {
  std::random_device random;
  std::uniform_int_distribution<unsigned int> byteDistribution{
      0, static_cast<unsigned int>(BYTE_MASK)};
  SipHashKey key{};
  std::ranges::generate(key, [&random, &byteDistribution] {
    return static_cast<std::byte>(byteDistribution(random));
  });
  return key;
}

std::uint64_t windowOf(std::chrono::nanoseconds now) {
  return static_cast<std::uint64_t>(now / COOKIE_WINDOW_DURATION);
}

}  // namespace

CookieIssuer::CookieIssuer() : CookieIssuer(randomKey()) {}

CookieIssuer::CookieIssuer(const SipHashKey& secret) : secret_(secret) {}

std::uint64_t CookieIssuer::issue(const Endpoint& sender,
                                  std::chrono::nanoseconds now) const {
  return cookieOf(sender, windowOf(now));
}

bool CookieIssuer::isValid(std::uint64_t cookie, const Endpoint& sender,
                           std::chrono::nanoseconds now) const {
  const std::uint64_t window = windowOf(now);
  return cookie != 0 && (cookie == cookieOf(sender, window) ||
                         cookie == cookieOf(sender, window - 1));
}

std::uint64_t CookieIssuer::cookieOf(const Endpoint& sender,
                                     std::uint64_t window) const {
  ByteWriter message;
  message.writeBytes(std::as_bytes(std::span{sender.address}));
  message.writeUint16(sender.port);
  message.writeUint64(window);
  return sipHash(secret_, message.bytes());
}

}  // namespace rtype::network
