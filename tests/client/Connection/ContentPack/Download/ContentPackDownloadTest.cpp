#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <vector>
#include "ByteText.hpp"
#include "ContentPackDownload.hpp"
#include "ContentPackDownloadFixtures.hpp"
#include "ContentPackDownloadTestConstants.hpp"
#include "FakePackServer.hpp"
#include "PackDownloadOutcome.hpp"
#include "PackDownloadProgress.hpp"
#include "ServedPack.hpp"

using rtype::client::ContentPackDownload;
using rtype::client::PackDownloadOutcome;
using rtype::client::PackDownloadProgress;
using rtype::client::testing::CUT_AFTER;
using rtype::client::testing::endOf;
using rtype::client::testing::lancerBody;
using rtype::client::testing::NOT_FOUND_STATUS;
using rtype::client::testing::ONE_REQUEST;
using rtype::client::testing::PACK_PATH;
using rtype::client::testing::packAt;
using rtype::client::testing::RANGE_FROM_THE_CUT;
using rtype::client::testing::SILENT_PORT;
using rtype::client::testing::TWO_REQUESTS;
using rtype::network::textOf;

/**
 * Given a server of a pack
 * When the pack is downloaded
 * Then every byte comes in one request
 */
TEST(ContentPackDownload, DownloadsAWholePack) {
  const std::string body = lancerBody();
  const FakePackServer server{
      ServedPack{.path = std::string{PACK_PATH}, .body = body}};
  ContentPackDownload download{packAt(server.port(), body.size())};

  const PackDownloadProgress end = endOf(download);

  EXPECT_EQ(end.outcome, PackDownloadOutcome::Finished);
  EXPECT_EQ(textOf(download.takeBytes()), body);
  EXPECT_EQ(server.requests(), ONE_REQUEST);
}

/**
 * Given a server that cuts its first answer after a few bytes
 * When the pack is downloaded
 * Then the second try asks only for the rest, and the pack comes whole
 */
TEST(ContentPackDownload, ResumesACutTryWhereItStopped) {
  const std::string body = lancerBody();
  FakePackServer server{
      ServedPack{.path = std::string{PACK_PATH}, .body = body}};
  server.cutTheFirstAnswerAfter(CUT_AFTER);
  ContentPackDownload download{packAt(server.port(), body.size())};

  const PackDownloadProgress end = endOf(download);

  EXPECT_EQ(end.outcome, PackDownloadOutcome::Finished);
  EXPECT_EQ(textOf(download.takeBytes()), body);
  EXPECT_EQ(server.requests(), TWO_REQUESTS);
  EXPECT_EQ(server.lastRange().value_or(std::string{}), RANGE_FROM_THE_CUT);
}

/**
 * Given a server that refuses the pack
 * When it is downloaded
 * Then the download fails at once, without trying again
 */
TEST(ContentPackDownload, FailsAtOnceOnARefusal) {
  FakePackServer server{
      ServedPack{.path = std::string{PACK_PATH}, .body = lancerBody()}};
  server.answerEveryRequestWith(NOT_FOUND_STATUS);
  ContentPackDownload download{packAt(server.port(), lancerBody().size())};

  const PackDownloadProgress end = endOf(download);

  EXPECT_EQ(end.outcome, PackDownloadOutcome::Failed);
  EXPECT_TRUE(download.takeBytes().empty());
  EXPECT_EQ(server.requests(), ONE_REQUEST);
}

/**
 * Given a server that sends one byte more than it announced
 * When the pack is downloaded
 * Then the download fails at once, keeping no more than the announced size
 */
TEST(ContentPackDownload, FailsWhenTheServerSendsMoreThanItAnnounced) {
  const std::string body = lancerBody();
  const FakePackServer server{
      ServedPack{.path = std::string{PACK_PATH}, .body = body}};
  const ContentPackDownload download{packAt(server.port(), body.size() - 1)};

  const PackDownloadProgress end = endOf(download);

  EXPECT_EQ(end.outcome, PackDownloadOutcome::Failed);
  EXPECT_LE(end.received, body.size() - 1);
  EXPECT_EQ(server.requests(), ONE_REQUEST);
}

/**
 * Given no server on the port
 * When a pack is downloaded from it
 * Then the download fails once its tries are spent
 */
TEST(ContentPackDownload, FailsWhenNothingAnswers) {
  const ContentPackDownload download{packAt(SILENT_PORT, lancerBody().size())};

  EXPECT_EQ(endOf(download).outcome, PackDownloadOutcome::Failed);
}
