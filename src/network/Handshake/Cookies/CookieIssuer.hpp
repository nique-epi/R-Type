#pragma once

#include <chrono>
#include <cstdint>
#include "CookieWindows.hpp"
#include "Endpoint.hpp"
#include "HandshakeConstants.hpp"
#include "SipHash.hpp"

namespace rtype::network {

/**
 * @brief Issues a cookie, and recognizes it when it comes back, without
 * keeping anything per request: the cookie of a ConnectionChallenge, and the
 * token of the probe of the master.
 *
 * A cookie is the SipHash, under a secret key, of an address, its port and the
 * number of the current window (time divided by the duration of a window).
 * Only the owner of an address receives what is sent to it, so whoever forges
 * that address cannot send the cookie back, and without the key nobody can
 * compute it. A cookie is valid in the window it was issued in and the
 * accepted - 1 windows after it.
 */
class CookieIssuer {
 public:
  /**
   * @brief Draws its secret key from std::random_device.
   * @param windows The windows of its cookies: by default, those of a
   * connection cookie.
   */
  explicit CookieIssuer(CookieWindows windows = CONNECTION_COOKIE_WINDOWS);

  /** @param secret The key the cookies are computed with.
   * @param windows As above. */
  explicit CookieIssuer(const SipHashKey& secret,
                        CookieWindows windows = CONNECTION_COOKIE_WINDOWS);

  /**
   * @returns The cookie of @p sender at time @p now. @p now is measured from
   * any fixed origin, the same for every call.
   */
  [[nodiscard]] std::uint64_t issue(const Endpoint& sender,
                                    std::chrono::nanoseconds now) const;

  /**
   * @returns Whether @p cookie is the cookie of @p sender in the window of
   * @p now or in one of the accepted - 1 windows before it. 0, the cookie of a
   * request that has none yet, is never valid.
   */
  [[nodiscard]] bool isValid(std::uint64_t cookie, const Endpoint& sender,
                             std::chrono::nanoseconds now) const;

 private:
  [[nodiscard]] std::uint64_t windowOf(std::chrono::nanoseconds now) const;
  [[nodiscard]] std::uint64_t cookieOf(const Endpoint& sender,
                                       std::uint64_t window) const;

  SipHashKey secret_;
  CookieWindows windows_;
};

}  // namespace rtype::network
