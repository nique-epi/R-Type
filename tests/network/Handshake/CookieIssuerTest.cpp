#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include "CookieIssuer.hpp"
#include "Endpoint.hpp"
#include "HandshakeConstants.hpp"
#include "HandshakeTestConstants.hpp"
#include "SipHash.hpp"

namespace rtype::network {
namespace {

using testing::END_OF_THAT_WINDOW;
using testing::END_OF_THE_NEXT_WINDOW;
using testing::OTHER_KEY_BYTE;
using testing::OTHER_SENDER_ADDRESS;
using testing::OTHER_SENDER_PORT;
using testing::PROTOCOL_COOKIE_WINDOW;
using testing::SENDER_ADDRESS;
using testing::SENDER_PORT;
using testing::START_OF_A_WINDOW;
using testing::START_OF_THE_NEXT_WINDOW;
using testing::TWO_WINDOWS_LATER;

Endpoint sender() {
  return Endpoint{.address = std::string{SENDER_ADDRESS}, .port = SENDER_PORT};
}

/**
 * Given the cookie window of the protocol page
 * When the window the issuer uses is read
 * Then it lasts 10 s
 */
TEST(CookieIssuer, UsesTheWindowOfTheProtocolPage) {
  EXPECT_EQ(COOKIE_WINDOW_DURATION, PROTOCOL_COOKIE_WINDOW);
}

/**
 * Given a cookie issued to a sender at the start of a window
 * When the same sender sends it back at the last moment of that window
 * Then it is valid
 */
TEST(CookieIssuer, AcceptsItsCookieUntilTheEndOfItsWindow) {
  const CookieIssuer issuer;
  const std::uint64_t cookie = issuer.issue(sender(), START_OF_A_WINDOW);

  EXPECT_TRUE(issuer.isValid(cookie, sender(), END_OF_THAT_WINDOW));
}

/**
 * Given a cookie issued at the last moment of a window
 * When it comes back at the last moment of the next window
 * Then it is still valid, so a challenge answered across a window end works
 */
TEST(CookieIssuer, AcceptsItsCookieDuringTheNextWindow) {
  const CookieIssuer issuer;
  const std::uint64_t cookie = issuer.issue(sender(), END_OF_THAT_WINDOW);

  EXPECT_TRUE(issuer.isValid(cookie, sender(), END_OF_THE_NEXT_WINDOW));
}

/**
 * Given a cookie issued in a window
 * When it comes back at the start of the window after the next one
 * Then it has expired
 */
TEST(CookieIssuer, RefusesItsCookieTwoWindowsLater) {
  const CookieIssuer issuer;
  const std::uint64_t cookie = issuer.issue(sender(), END_OF_THAT_WINDOW);

  EXPECT_FALSE(issuer.isValid(cookie, sender(), TWO_WINDOWS_LATER));
}

/**
 * Given a cookie issued in a window
 * When it is checked at a time before that window
 * Then it is not valid, since it was not issued yet then
 */
TEST(CookieIssuer, RefusesACookieFromAWindowNotReachedYet) {
  const CookieIssuer issuer;
  const std::uint64_t cookie = issuer.issue(sender(), START_OF_THE_NEXT_WINDOW);

  EXPECT_FALSE(issuer.isValid(cookie, sender(), END_OF_THAT_WINDOW));
}

/**
 * Given a cookie issued to a sender
 * When a sender on the same address but another port sends it back
 * Then it is not valid
 */
TEST(CookieIssuer, RefusesTheCookieOfAnotherPort) {
  const CookieIssuer issuer;
  const std::uint64_t cookie = issuer.issue(sender(), START_OF_A_WINDOW);
  const Endpoint otherPort{.address = std::string{SENDER_ADDRESS},
                           .port = OTHER_SENDER_PORT};

  EXPECT_FALSE(issuer.isValid(cookie, otherPort, START_OF_A_WINDOW));
}

/**
 * Given a cookie issued to a sender
 * When a sender on another address and the same port sends it back
 * Then it is not valid
 */
TEST(CookieIssuer, RefusesTheCookieOfAnotherAddress) {
  const CookieIssuer issuer;
  const std::uint64_t cookie = issuer.issue(sender(), START_OF_A_WINDOW);
  const Endpoint otherAddress{.address = std::string{OTHER_SENDER_ADDRESS},
                              .port = SENDER_PORT};

  EXPECT_FALSE(issuer.isValid(cookie, otherAddress, START_OF_A_WINDOW));
}

/**
 * Given two issuers whose keys differ by one byte
 * When the cookie of one is checked by the other
 * Then it is not valid: the cookie depends on the secret key
 */
TEST(CookieIssuer, RefusesACookieIssuedUnderAnotherKey) {
  SipHashKey otherKey{};
  otherKey.front() = OTHER_KEY_BYTE;
  const CookieIssuer issuer{SipHashKey{}};
  const CookieIssuer otherIssuer{otherKey};
  const std::uint64_t cookie = otherIssuer.issue(sender(), START_OF_A_WINDOW);

  EXPECT_FALSE(issuer.isValid(cookie, sender(), START_OF_A_WINDOW));
}

/**
 * Given any issuer and sender
 * When the cookie 0, which a request without a cookie carries, is checked
 * Then it is not valid
 */
TEST(CookieIssuer, RefusesTheMissingCookie) {
  const CookieIssuer issuer;

  EXPECT_FALSE(issuer.isValid(0, sender(), START_OF_A_WINDOW));
}

}  // namespace
}  // namespace rtype::network
