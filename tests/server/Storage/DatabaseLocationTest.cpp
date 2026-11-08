#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include "DatabaseLocationFixture.hpp"
#include "DatabaseLocationTestConstants.hpp"
#include "ServerStorageModuleTestConstants.hpp"

/**
 * Given a location whose database setting is empty
 * When its file is asked
 * Then it is the file of the port of the server in the folder
 */
TEST_F(DatabaseLocationFixture, AnEmptySettingGivesTheFileOfThePort) {
  EXPECT_EQ(location().file(),
            std::filesystem::path{std::string{LOCATION_FOLDER}} /
                std::string{LISTENING_PORT_DATABASE_FILE});
}

/**
 * Given a location built before the configuration ran
 * When a line of the configuration sets the database, then its file is asked
 * Then it is the file the line named: the setting is read at each call
 */
TEST_F(DatabaseLocationFixture, ReadsTheSettingTheConfigurationGaveLater) {
  run(SET_DATABASE_LINE);

  EXPECT_EQ(location().file(),
            std::filesystem::path{std::string{SETTING_DATABASE_FILE}});
}
