#pragma once

#include <chrono>
#include <cstdint>
#include "Endpoint.hpp"
#include "SipHash.hpp"

namespace rtype::network {

/**
 * @brief Issues the cookie of a ConnectionChallenge, and recognizes it when the
 * sender sends it back, without keeping anything per request.
 *
 * A cookie is the SipHash, under a secret key, of the sender's address, its
 * port and the number of the current cookie window (time divided by
 * COOKIE_WINDOW_DURATION). Only the owner of an address receives the challenge
 * sent to it, so whoever forges that address cannot send the cookie back, and
 * without the key nobody can compute it. A cookie is valid during the window
 * it was issued in and the next one: it lives one to two windows, so a
 * challenge answered just before a window ends still works.
 */
class CookieIssuer {
 public:
  /** @brief Draws its secret key from std::random_device. */
  CookieIssuer();

  /** @param secret The key the cookies are computed with. */
  explicit CookieIssuer(const SipHashKey& secret);

  /**
   * @returns The cookie of @p sender at time @p now. @p now is measured from
   * any fixed origin, the same for every call.
   */
  [[nodiscard]] std::uint64_t issue(const Endpoint& sender,
                                    std::chrono::nanoseconds now) const;

  /**
   * @returns Whether @p cookie is the cookie of @p sender in the window of
   * @p now or in the window before it. 0, the cookie of a request that has
   * none yet, is never valid.
   */
  [[nodiscard]] bool isValid(std::uint64_t cookie, const Endpoint& sender,
                             std::chrono::nanoseconds now) const;

 private:
  [[nodiscard]] std::uint64_t cookieOf(const Endpoint& sender,
                                       std::uint64_t window) const;

  SipHashKey secret_;
};

}  // namespace rtype::network
