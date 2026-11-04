#include <gtest/gtest.h>
#include "Application.hpp"
#include "ServerLoggerModuleFixture.hpp"
#include "ServerLoggerModuleTestConstants.hpp"

using rtype::engine::Application;

/**
 * Given an application made of the logger module only
 * When it is built and nothing is started
 * Then the journal file has no line: opening the journal waits for start()
 */
TEST_F(ServerLoggerModuleFixture, WritesNothingBeforeItIsStarted) {
  const Application application = applicationWithTheLogger();

  EXPECT_TRUE(journal().lines().empty());
}

/**
 * Given the logger module started
 * When the application has started
 * Then the journal says that the server starts
 */
TEST_F(ServerLoggerModuleFixture, WritesThatTheServerStartsWhenStarted) {
  Application application = applicationWithTheLogger();

  application.start();

  EXPECT_FALSE(journal().linesHolding(SERVER_STARTING_LINE_TEXT).empty());
}

/**
 * Given the logger module started
 * When the application stops
 * Then the journal says that the server stopped
 */
TEST_F(ServerLoggerModuleFixture, WritesThatTheServerStoppedWhenStopped) {
  Application application = applicationWithTheLogger();
  application.start();

  application.stop();

  EXPECT_FALSE(journal().linesHolding(SERVER_STOPPED_LINE_TEXT).empty());
}
