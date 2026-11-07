#include <gtest/gtest.h>
#include <httplib.h>
#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <vector>
#include "ContentFile.hpp"
#include "ContentPack.hpp"
#include "ContentPackRoute.hpp"
#include "ServerContentModuleFixtures.hpp"
#include "ServerContentModuleTestConstants.hpp"
#include "SupervisorModulesFixture.hpp"

using rtype::identifiers::ContentFile;
using rtype::network::contentFingerprintOf;
using rtype::network::contentPackPathOf;
using rtype::network::decodeContentPack;
using rtype::server::testing::BYTES_ALREADY_RECEIVED;
using rtype::server::testing::DRIFTER_SHEET;
using rtype::server::testing::PARTIAL_ANSWER;
using rtype::server::testing::WHOLE_ANSWER;

/**
 * Given a started supervisor
 * When a player asks the TCP port of its number for the pack of its assets
 * Then it gets a pack that holds the sheets, and whose fingerprint is the one
 * it asked for
 */
TEST_F(SupervisorModulesFixture, ServesThePackOfItsAssetsOverHttp) {
  const auto application = supervisor({});
  application->start();
  const std::string path = pathOfTheLocalPack();

  const httplib::Result answer = fetchedFrom(port(), path);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, WHOLE_ANSWER);
  const std::span<const std::byte> pack =
      std::as_bytes(std::span{answer->body.data(), answer->body.size()});
  EXPECT_EQ(contentPackPathOf(contentFingerprintOf(pack)), path);
  const std::vector<ContentFile> files = decodeContentPack(pack);
  EXPECT_EQ(std::ranges::count(files, DRIFTER_SHEET, &ContentFile::path), 1);
  application->stop();
}

/**
 * Given a started supervisor and a download cut after a few bytes
 * When the player asks for the rest of the pack
 * Then it gets only the bytes it lacks, which end the pack
 */
TEST_F(SupervisorModulesFixture, ResumesACutDownload) {
  const auto application = supervisor({});
  application->start();
  const std::string path = pathOfTheLocalPack();
  const httplib::Result whole = fetchedFrom(port(), path);
  ASSERT_TRUE(whole);

  const httplib::Result rest =
      fetchedFromOffset(port(), path, BYTES_ALREADY_RECEIVED);

  ASSERT_TRUE(rest);
  EXPECT_EQ(rest->status, PARTIAL_ANSWER);
  EXPECT_EQ(rest->body, whole->body.substr(BYTES_ALREADY_RECEIVED));
  application->stop();
}
