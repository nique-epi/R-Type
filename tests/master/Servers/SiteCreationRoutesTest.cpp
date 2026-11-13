#include <gtest/gtest.h>
#include <string>
#include "MasterTestConstants.hpp"
#include "RunningMaster.hpp"
#include "SiteClaim.hpp"
#include "SiteClaimTestConstants.hpp"
#include "SiteCreationRoutesFixtures.hpp"

using rtype::master::SiteClaim;

/**
 * Given a master holding the site key, and a visitor of the site who created
 * the five servers of an hour
 * When the site creates a sixth one for that visitor
 * Then it gets 429: the quota applies to the visitor the site names
 */
TEST(SiteCreationRoutes, ASixthCreationForOneVisitorIsTooManyRequests) {
  const RunningMaster master{siteKeyLines()};
  const SiteClaim visitor{.key = std::string{TEST_SITE_KEY},
                          .clientAddress = std::string{FIRST_VISITOR}};
  createEveryServerOfAnHour(master.port(), visitor);

  EXPECT_EQ(siteCreationStatus(master.port(), visitor),
            EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given a master holding the site key, and a visitor of the site who created
 * the five servers of an hour
 * When the site creates one for another visitor, from the same address
 * Then it is created: each visitor has a quota of its own
 */
TEST(SiteCreationRoutes, AnotherVisitorOfTheSiteStillCreates) {
  const RunningMaster master{siteKeyLines()};
  createEveryServerOfAnHour(
      master.port(), SiteClaim{.key = std::string{TEST_SITE_KEY},
                               .clientAddress = std::string{FIRST_VISITOR}});

  EXPECT_EQ(siteCreationStatus(
                master.port(),
                SiteClaim{.key = std::string{TEST_SITE_KEY},
                          .clientAddress = std::string{SECOND_VISITOR}}),
            EXPECTED_CREATED);
}

/**
 * Given a master holding the site key, and five creations sent with another
 * key and the address of a visitor
 * When a sixth one names another visitor with that wrong key
 * Then it gets 429: all six are counted under the address they came from
 */
TEST(SiteCreationRoutes, AWrongKeyIsCountedUnderItsOwnAddress) {
  const RunningMaster master{siteKeyLines()};
  createEveryServerOfAnHour(
      master.port(), SiteClaim{.key = std::string{WRONG_SITE_KEY},
                               .clientAddress = std::string{FIRST_VISITOR}});

  EXPECT_EQ(siteCreationStatus(
                master.port(),
                SiteClaim{.key = std::string{WRONG_SITE_KEY},
                          .clientAddress = std::string{SECOND_VISITOR}}),
            EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given a master holding the site key, and five creations naming a visitor
 * with no key
 * When a sixth one names another visitor with no key
 * Then it gets 429: all six are counted under the address they came from
 */
TEST(SiteCreationRoutes, AMissingKeyIsCountedUnderItsOwnAddress) {
  const RunningMaster master{siteKeyLines()};
  createEveryServerOfAnHour(
      master.port(), SiteClaim{.key = std::string{NO_SITE_KEY},
                               .clientAddress = std::string{FIRST_VISITOR}});

  EXPECT_EQ(siteCreationStatus(
                master.port(),
                SiteClaim{.key = std::string{NO_SITE_KEY},
                          .clientAddress = std::string{SECOND_VISITOR}}),
            EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given a master with no site key, and five creations sent with a key and
 * the address of a visitor
 * When a sixth one names another visitor with that key
 * Then it gets 429: a master without the setting reads neither header
 */
TEST(SiteCreationRoutes, AMasterWithoutTheSettingIgnoresTheHeaders) {
  const RunningMaster master;
  createEveryServerOfAnHour(
      master.port(), SiteClaim{.key = std::string{TEST_SITE_KEY},
                               .clientAddress = std::string{FIRST_VISITOR}});

  EXPECT_EQ(siteCreationStatus(
                master.port(),
                SiteClaim{.key = std::string{TEST_SITE_KEY},
                          .clientAddress = std::string{SECOND_VISITOR}}),
            EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given a master holding the site key, and five creations of the site whose
 * visitor is a text that names no address
 * When the site sends a sixth one, with no visitor at all
 * Then it gets 429: all six fell back on the address they came from
 */
TEST(SiteCreationRoutes, AVisitorThatIsNoAddressFallsBackOnThePeer) {
  const RunningMaster master{siteKeyLines()};
  createEveryServerOfAnHour(
      master.port(), SiteClaim{.key = std::string{TEST_SITE_KEY},
                               .clientAddress = std::string{NOT_AN_ADDRESS}});

  EXPECT_EQ(siteCreationStatus(master.port(),
                               SiteClaim{.key = std::string{TEST_SITE_KEY},
                                         .clientAddress = {}}),
            EXPECTED_TOO_MANY_REQUESTS);
}
