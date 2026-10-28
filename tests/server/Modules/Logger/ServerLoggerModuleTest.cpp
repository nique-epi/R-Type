#include <gtest/gtest.h>
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "IModule.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerModuleFixture.hpp"

using rtype::engine::Application;
using rtype::server::ServerLoggerModule;

namespace {

bool journalHolds(const std::vector<std::string>& lines,
                  const std::string& text) {
  return std::ranges::any_of(lines, [&text](const std::string& line) {
    return line.find(text) != std::string::npos;
  });
}

}  // namespace

class ServerLoggerModuleTest : public ServerModuleFixture {
 protected:
  [[nodiscard]] Application applicationWithTheLogger() const {
    std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
    modules.push_back(std::make_unique<ServerLoggerModule>(journalOptions()));
    return Application(std::move(modules));
  }
};

/**
 * Given an application made of the logger module only
 * When it is built and nothing is started
 * Then the journal file has no line: opening the journal waits for start()
 */
TEST_F(ServerLoggerModuleTest, WritesNothingBeforeItIsStarted) {
  const Application application = applicationWithTheLogger();

  EXPECT_TRUE(journal().lines().empty());
}

/**
 * Given the logger module started
 * When the application has started
 * Then the journal says that the server starts
 */
TEST_F(ServerLoggerModuleTest, WritesThatTheServerStartsWhenStarted) {
  Application application = applicationWithTheLogger();

  application.start();

  EXPECT_TRUE(journalHolds(journal().lines(), "server starting"));
}

/**
 * Given the logger module started
 * When the application stops
 * Then the journal says that the server stopped
 */
TEST_F(ServerLoggerModuleTest, WritesThatTheServerStoppedWhenStopped) {
  Application application = applicationWithTheLogger();
  application.start();

  application.stop();

  EXPECT_TRUE(journalHolds(journal().lines(), "server stopped"));
}
