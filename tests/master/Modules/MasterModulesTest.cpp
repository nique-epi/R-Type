#include <gtest/gtest.h>
#include <httplib.h>
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "HttpPortProbe.hpp"
#include "IModule.hpp"
#include "MasterConstants.hpp"
#include "MasterModulesFixture.hpp"
#include "MasterModulesTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "RouteAddingModule.hpp"

using rtype::engine::IModule;
using rtype::engine::ModuleStartFailedException;
using rtype::master::MASTER_VERSION;

/**
 * Given the modules of the master
 * When the master starts and a client asks GET /api/health
 * Then the master answers 200 with its version
 */
TEST_F(MasterModulesFixture, TheMasterAnswersItsVersionOnceStarted) {
  const auto application = masterApplication({}, {});

  application->start();
  const httplib::Result answer = get(HEALTH_URL);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(VERSION_MEMBER_NAME)
                .get<std::string>(),
            MASTER_VERSION);
}

/**
 * Given the `database` setting naming a file of an empty folder
 * When the master starts, then stops
 * Then that file exists: the master created its database
 */
TEST_F(MasterModulesFixture, TheMasterCreatesItsDatabaseWhereTheSettingSays) {
  const auto application = masterApplication({}, {});

  application->start();
  application->stop();

  EXPECT_TRUE(std::filesystem::exists(folder() / TEST_DATABASE_FILE));
}

/**
 * Given the `database` setting naming a file in a folder that does not exist
 * When the master starts
 * Then the start fails, and the port of the API was never opened: the
 * storage starts before the API
 */
TEST_F(MasterModulesFixture, ADatabaseItCannotOpenStopsTheMasterBeforeItsPort) {
  const auto application = masterApplication(
      {}, {databaseLine(folder() / MISSING_FOLDER / TEST_DATABASE_FILE)});

  EXPECT_THROW(application->start(), ModuleStartFailedException);

  EXPECT_TRUE(httpPortIsFree(port()));
}

/**
 * Given the module of a feature listed after the modules of the master, which
 * adds its route to the HTTP server while it installs
 * When the master starts and a client calls that route
 * Then the master answers it
 */
TEST_F(MasterModulesFixture, AModuleListedAfterTheHttpModuleAddsItsRoute) {
  std::vector<std::unique_ptr<IModule>> feature;
  feature.push_back(
      std::make_unique<RouteAddingModule>(std::string{FEATURE_ROUTE}));
  const auto application = masterApplication(std::move(feature), {});

  application->start();
  const httplib::Result answer = get(FEATURE_URL);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
}
