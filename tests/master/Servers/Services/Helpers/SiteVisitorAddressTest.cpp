#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
#include "SiteClaim.hpp"
#include "SiteClaimTestConstants.hpp"
#include "SiteVisitorAddress.hpp"

using rtype::master::SiteClaim;
using rtype::master::visitorAddressOf;

/**
 * Given a request carrying the site key and an IPv4 visitor
 * When the master holding that key reads its visitor
 * Then it is the visitor the site names
 */
TEST(SiteVisitorAddress, TheSiteKeyNamesTheVisitor) {
  EXPECT_EQ(
      visitorAddressOf(SiteClaim{.key = std::string{TEST_SITE_KEY},
                                 .clientAddress = std::string{FIRST_VISITOR}},
                       TEST_SITE_KEY),
      std::optional<std::string>{FIRST_VISITOR});
}

/**
 * Given a request carrying the site key and an IPv6 visitor
 * When the master holding that key reads its visitor
 * Then it is the visitor the site names
 */
TEST(SiteVisitorAddress, TheSiteKeyNamesAnIpv6Visitor) {
  EXPECT_EQ(
      visitorAddressOf(SiteClaim{.key = std::string{TEST_SITE_KEY},
                                 .clientAddress = std::string{IPV6_VISITOR}},
                       TEST_SITE_KEY),
      std::optional<std::string>{IPV6_VISITOR});
}

/**
 * Given a request carrying another key than the site's, and a visitor
 * When the master reads its visitor
 * Then it has none: a client without the secret does not choose its address
 */
TEST(SiteVisitorAddress, AWrongKeyNamesNoVisitor) {
  EXPECT_EQ(
      visitorAddressOf(SiteClaim{.key = std::string{WRONG_SITE_KEY},
                                 .clientAddress = std::string{FIRST_VISITOR}},
                       TEST_SITE_KEY),
      std::nullopt);
}

/**
 * Given a request naming a visitor with no key
 * When the master holding a key reads its visitor
 * Then it has none
 */
TEST(SiteVisitorAddress, AMissingKeyNamesNoVisitor) {
  EXPECT_EQ(
      visitorAddressOf(SiteClaim{.key = std::string{NO_SITE_KEY},
                                 .clientAddress = std::string{FIRST_VISITOR}},
                       TEST_SITE_KEY),
      std::nullopt);
}

/**
 * Given a master whose site key setting is empty
 * When a request with an empty key names a visitor
 * Then it has none: an empty setting believes nobody, an empty key included
 */
TEST(SiteVisitorAddress, AnEmptySettingBelievesNoKey) {
  EXPECT_EQ(
      visitorAddressOf(SiteClaim{.key = std::string{NO_SITE_KEY},
                                 .clientAddress = std::string{FIRST_VISITOR}},
                       NO_SITE_KEY),
      std::nullopt);
}

/**
 * Given the site key and a text that names no address
 * When the master reads its visitor
 * Then it has none, for each such text
 */
TEST(SiteVisitorAddress, ATextThatIsNoAddressNamesNoVisitor) {
  for (const std::string_view text : TEXTS_THAT_ARE_NO_ADDRESS) {
    EXPECT_EQ(visitorAddressOf(SiteClaim{.key = std::string{TEST_SITE_KEY},
                                         .clientAddress = std::string{text}},
                               TEST_SITE_KEY),
              std::nullopt)
        << text;
  }
}

/**
 * Given the site key and the longest text an address has, 45 characters
 * When the master reads its visitor
 * Then it is that address
 */
TEST(SiteVisitorAddress, TheLongestAddressNamesTheVisitor) {
  EXPECT_EQ(
      visitorAddressOf(SiteClaim{.key = std::string{TEST_SITE_KEY},
                                 .clientAddress = std::string{LONGEST_ADDRESS}},
                       TEST_SITE_KEY),
      std::optional<std::string>{LONGEST_ADDRESS});
}

/**
 * Given the site key and an address of 46 characters
 * When the master reads its visitor
 * Then it has none: the text is longer than any address
 */
TEST(SiteVisitorAddress, AnAddressOneCharacterTooLongNamesNoVisitor) {
  EXPECT_EQ(visitorAddressOf(
                SiteClaim{.key = std::string{TEST_SITE_KEY},
                          .clientAddress =
                              std::string{ADDRESS_ONE_CHARACTER_TOO_LONG}},
                TEST_SITE_KEY),
            std::nullopt);
}
