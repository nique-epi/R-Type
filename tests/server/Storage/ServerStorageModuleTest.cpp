#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include "EngineException.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerStorageModuleFixture.hpp"
#include "ServerStorageModuleTestConstants.hpp"

using rtype::engine::MissingDependencyException;
using rtype::engine::ModuleStartFailedException;

/**
 * Given the storage module with no network module listed before it
 * When the application is built
 * Then it is refused: the file is named after the port of the network module,
 * which nothing would give
 */
TEST_F(ServerStorageModuleFixture,
       TheStorageModuleNeedsTheNetworkModuleBeforeIt) {
  EXPECT_THROW((void)storageApplication(std::nullopt, {}),
               MissingDependencyException);
}

/**
 * Given a module before it that gives a port, and the `database` setting left
 * empty
 * When the storage module starts, then stops
 * Then the file is named after that port, so two servers never share one
 */
TEST_F(ServerStorageModuleFixture,
       TheStorageModuleNamesTheFileAfterThePortItIsGiven) {
  const auto application = storageApplication(LISTENING_PORT, {});

  application->start();
  application->stop();

  EXPECT_TRUE(std::filesystem::exists(folder() / LISTENING_PORT_DATABASE_FILE));
  EXPECT_FALSE(std::filesystem::exists(folder() / DEFAULT_PORT_DATABASE_FILE));
}

/**
 * Given the `database` setting naming a file, and a port given
 * When the storage module starts, then stops
 * Then it opened that file, and not the file of the port
 */
TEST_F(ServerStorageModuleFixture,
       TheStorageModuleOpensTheFileTheDatabaseSettingNames) {
  const auto application = storageApplication(
      LISTENING_PORT, {databaseLine(folder() / TEST_DATABASE_FILE)});

  application->start();
  application->stop();

  EXPECT_TRUE(std::filesystem::exists(folder() / TEST_DATABASE_FILE));
  EXPECT_FALSE(
      std::filesystem::exists(folder() / LISTENING_PORT_DATABASE_FILE));
}

/**
 * Given the `database` setting naming a file in a folder that does not exist
 * When the application starts
 * Then the start fails with an error that names the storage module
 */
TEST_F(ServerStorageModuleFixture,
       TheStorageModuleStopsTheStartWhenItCannotOpenItsFile) {
  const auto application = storageApplication(
      LISTENING_PORT,
      {databaseLine(folder() / MISSING_FOLDER / TEST_DATABASE_FILE)});

  EXPECT_THROW(application->start(), ModuleStartFailedException);
}
