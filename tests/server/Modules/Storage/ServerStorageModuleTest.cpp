#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "EngineException.hpp"
#include "IModule.hpp"
#include "LoopStages.hpp"
#include "ResultsDatabase.hpp"
#include "ServerConstants.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerModuleFixture.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerStorageModule.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::Application;
using rtype::engine::LoopStages;
using rtype::engine::ModuleStartFailedException;
using rtype::server::ResultsDatabase;
using rtype::server::ServerLoggerModule;
using rtype::server::ServerStorageModule;

/**
 * Given a file that does not exist
 * When the module starts, then stops
 * Then the file exists and holds the tables of the results
 */
TEST_F(ServerModuleFixture, TheStorageModuleCreatesTheDatabaseWhenItStarts) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / TEST_DATABASE_FILE;
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerLoggerModule>(journalOptions()));
  modules.push_back(std::make_unique<ServerStorageModule>(file));
  Application application(LoopStages(rtype::server::SERVER_LOOP_STAGES),
                          std::move(modules));

  application.start();
  application.stop();

  EXPECT_NO_THROW(ResultsDatabase{file});
}

/**
 * Given a file that cannot be created
 * When the application starts
 * Then the start fails with an error that names the storage module
 */
TEST_F(ServerModuleFixture,
       TheStorageModuleStopsTheStartWhenItCannotOpenItsFile) {
  const TemporaryFolder folder;
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerLoggerModule>(journalOptions()));
  modules.push_back(std::make_unique<ServerStorageModule>(
      folder.path() / "no-such-folder" / TEST_DATABASE_FILE));
  Application application(LoopStages(rtype::server::SERVER_LOOP_STAGES),
                          std::move(modules));

  EXPECT_THROW(application.start(), ModuleStartFailedException);
}
