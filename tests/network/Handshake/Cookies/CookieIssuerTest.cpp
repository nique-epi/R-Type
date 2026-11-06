#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include "CookieIssuer.hpp"
#include "CookieIssuerFixtures.hpp"
#include "CookieWindows.hpp"
#include "Endpoint.hpp"
#include "HandshakeConstants.hpp"
#include "HandshakeTestConstants.hpp"
#include "SipHash.hpp"

using rtype::network::COOKIE_WINDOW_DURATION;
using rtype::network::CookieIssuer;
using rtype::network::Endpoint;
using rtype::network::SipHashKey;
using rtype::network::testing::END_OF_THAT_WINDOW;
using rtype::network::testing::END_OF_THE_NEXT_WINDOW;
using rtype::network::testing::FIRST_SECOND_OF_THE_FOURTH_WINDOW;
using rtype::network::testing::LAST_SECOND_OF_THE_THIRD_WINDOW;
using rtype::network::testing::OTHER_KEY_BYTE;
using rtype::network::testing::OTHER_SENDER_ADDRESS;
using rtype::network::testing::OTHER_SENDER_PORT;
using rtype::network::testing::PROBE_LIKE_WINDOWS;
using rtype::network::testing::PROTOCOL_COOKIE_WINDOW;
using rtype::network::testing::sender;
using rtype::network::testing::SENDER_ADDRESS;
using rtype::network::testing::SENDER_PORT;
using rtype::network::testing::START_OF_A_WINDOW;
using rtype::network::testing::START_OF_THE_NEXT_WINDOW;
using rtype::network::testing::TOKEN_ISSUED_AT;
using rtype::network::testing::TWO_WINDOWS_LATER;

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

/**
 * Given an issuer of windows of 5 s that accepts three, and a token issued at
 * the start of a window
 * When the token comes back at the last second of the third window
 * Then it is valid
 */
TEST(CookieIssuer, AcceptsATokenUpToItsLastWindow) {
  const CookieIssuer issuer{PROBE_LIKE_WINDOWS};
  const std::uint64_t token = issuer.issue(sender(), TOKEN_ISSUED_AT);

  EXPECT_TRUE(issuer.isValid(token, sender(), LAST_SECOND_OF_THE_THIRD_WINDOW));
}

/**
 * Given the same issuer and token
 * When the token comes back at the first second of the fourth window
 * Then it is refused
 */
TEST(CookieIssuer, RefusesATokenPastItsLastWindow) {
  const CookieIssuer issuer{PROBE_LIKE_WINDOWS};
  const std::uint64_t token = issuer.issue(sender(), TOKEN_ISSUED_AT);

  EXPECT_FALSE(
      issuer.isValid(token, sender(), FIRST_SECOND_OF_THE_FOURTH_WINDOW));
}
