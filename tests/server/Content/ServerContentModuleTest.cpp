#include <gtest/gtest.h>
#include <httplib.h>
#include <string>
#include "ServerContentModuleFixtures.hpp"
#include "ServerContentModuleTestConstants.hpp"
#include "SupervisorModulesFixture.hpp"

using rtype::server::testing::BYTES_ALREADY_RECEIVED;
using rtype::server::testing::PARTIAL_ANSWER;

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
