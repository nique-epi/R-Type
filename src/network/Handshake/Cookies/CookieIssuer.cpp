#include "CookieIssuer.hpp"
#include <chrono>
#include <cstdint>
#include "ByteWriter.hpp"
#include "CookieWindows.hpp"
#include "Endpoint.hpp"
#include "EndpointBytes.hpp"
#include "SipHash.hpp"

namespace rtype::network {

CookieIssuer::CookieIssuer(CookieWindows windows)
    : CookieIssuer(randomSipHashKey(), windows) {}

CookieIssuer::CookieIssuer(const SipHashKey& secret, CookieWindows windows)
    : secret_(secret), windows_(windows) {}

std::uint64_t CookieIssuer::issue(const Endpoint& sender,
                                  std::chrono::nanoseconds now) const {
  return cookieOf(sender, windowOf(now));
}

bool CookieIssuer::isValid(std::uint64_t cookie, const Endpoint& sender,
                           std::chrono::nanoseconds now) const {
  if (cookie == 0) {
    return false;
  }
  const std::uint64_t window = windowOf(now);
  for (std::uint64_t age = 0; age < windows_.accepted && age <= window; ++age) {
    if (cookie == cookieOf(sender, window - age)) {
      return true;
    }
  }
  return false;
}

std::uint64_t CookieIssuer::windowOf(std::chrono::nanoseconds now) const {
  return static_cast<std::uint64_t>(now / windows_.duration);
}

std::uint64_t CookieIssuer::cookieOf(const Endpoint& sender,
                                     std::uint64_t window) const {
  ByteWriter message;
  writeEndpoint(message, sender);
  message.writeUint64(window);
  return sipHash(secret_, message.bytes());
}

}  // namespace rtype::network
