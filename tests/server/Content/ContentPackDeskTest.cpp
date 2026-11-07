#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <vector>
#include "ApiResponse.hpp"
#include "ByteText.hpp"
#include "ContentFingerprintText.hpp"
#include "ContentPack.hpp"
#include "ContentPackDesk.hpp"
#include "ContentPackDeskFixtures.hpp"
#include "ContentPackDeskTestConstants.hpp"
#include "HttpHeader.hpp"
#include "SimulatedClock.hpp"

using rtype::network::ApiResponse;
using rtype::network::contentFingerprintOf;
using rtype::network::fingerprintText;
using rtype::network::HttpHeader;
using rtype::network::textOf;
using rtype::server::ContentPackDesk;
using rtype::server::testing::ETAG;
using rtype::server::testing::ETAG_QUOTE;
using rtype::server::testing::NOT_FOUND;
using rtype::server::testing::OCTET_STREAM;
using rtype::server::testing::OK;
using rtype::server::testing::OTHER_PLAYER_ADDRESS;
using rtype::server::testing::packRequest;
using rtype::server::testing::PLAYER_ADDRESS;
using rtype::server::testing::REQUESTS_AT_ONCE;
using rtype::server::testing::somePack;
using rtype::server::testing::TIME_BETWEEN_REQUESTS;
using rtype::server::testing::TOO_MANY_REQUESTS;
using rtype::server::testing::UNKNOWN_FINGERPRINT;

/**
 * Given a desk of a pack
 * When a player asks for the pack by its fingerprint
 * Then it gets every byte of the pack, as bytes, versioned by the fingerprint
 */
TEST(ContentPackDesk, ServesThePackByItsFingerprint) {
  const SimulatedClock clock;
  const std::vector<std::byte> pack = somePack();
  const std::string fingerprint = fingerprintText(contentFingerprintOf(pack));
  ContentPackDesk desk{pack, clock};

  const ApiResponse answer =
      desk.answer(packRequest(PLAYER_ADDRESS, fingerprint));

  EXPECT_EQ(answer.status, OK);
  EXPECT_EQ(answer.body, textOf(pack));
  EXPECT_EQ(answer.contentType, OCTET_STREAM);
  EXPECT_EQ(answer.header.value_or(HttpHeader{}).name, ETAG);
  EXPECT_EQ(answer.header.value_or(HttpHeader{}).value,
            std::string{ETAG_QUOTE} + fingerprint + std::string{ETAG_QUOTE});
}

/**
 * Given a desk of a pack
 * When a player asks for a pack of another fingerprint
 * Then it is told there is none, and gets no byte
 */
TEST(ContentPackDesk, AnswersAnotherFingerprintWithNotFound) {
  const SimulatedClock clock;
  ContentPackDesk desk{somePack(), clock};

  const ApiResponse answer =
      desk.answer(packRequest(PLAYER_ADDRESS, UNKNOWN_FINGERPRINT));

  EXPECT_EQ(answer.status, NOT_FOUND);
  EXPECT_TRUE(answer.body.empty());
}

/**
 * Given a player that asked for the pack as many times as it may at once
 * When it asks once more at the same time
 * Then it is refused, and gets no byte
 */
TEST(ContentPackDesk, RefusesOneRequestMoreThanItAllowsAtOnce) {
  const SimulatedClock clock;
  const std::vector<std::byte> pack = somePack();
  const std::string fingerprint = fingerprintText(contentFingerprintOf(pack));
  ContentPackDesk desk{pack, clock};
  for (std::size_t request = 0; request < REQUESTS_AT_ONCE; ++request) {
    ASSERT_EQ(desk.answer(packRequest(PLAYER_ADDRESS, fingerprint)).status, OK);
  }

  const ApiResponse answer =
      desk.answer(packRequest(PLAYER_ADDRESS, fingerprint));

  EXPECT_EQ(answer.status, TOO_MANY_REQUESTS);
  EXPECT_TRUE(answer.body.empty());
}

/**
 * Given a player that asked for the pack as many times as it may at once
 * When another address asks for it
 * Then that address gets the pack: each address has its own allowance
 */
TEST(ContentPackDesk, AnotherAddressKeepsItsOwnAllowance) {
  const SimulatedClock clock;
  const std::vector<std::byte> pack = somePack();
  const std::string fingerprint = fingerprintText(contentFingerprintOf(pack));
  ContentPackDesk desk{pack, clock};
  for (std::size_t request = 0; request < REQUESTS_AT_ONCE; ++request) {
    static_cast<void>(desk.answer(packRequest(PLAYER_ADDRESS, fingerprint)));
  }

  EXPECT_EQ(desk.answer(packRequest(OTHER_PLAYER_ADDRESS, fingerprint)).status,
            OK);
}

/**
 * Given a player that asked for the pack as many times as it may at once
 * When the time between two requests has passed
 * Then it may ask once more
 */
TEST(ContentPackDesk, AllowsOneMoreRequestOnceItsTimePassed) {
  SimulatedClock clock;
  const std::vector<std::byte> pack = somePack();
  const std::string fingerprint = fingerprintText(contentFingerprintOf(pack));
  ContentPackDesk desk{pack, clock};
  for (std::size_t request = 0; request < REQUESTS_AT_ONCE; ++request) {
    static_cast<void>(desk.answer(packRequest(PLAYER_ADDRESS, fingerprint)));
  }

  clock.advance(TIME_BETWEEN_REQUESTS);

  EXPECT_EQ(desk.answer(packRequest(PLAYER_ADDRESS, fingerprint)).status, OK);
}
