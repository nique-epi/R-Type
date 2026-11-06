#include <gtest/gtest.h>
#include <httplib.h>
#include <filesystem>
#include <memory>
#include <string>
#include "CurrentDirectoryChange.hpp"
#include "MasterConstants.hpp"
#include "MasterModulesFixture.hpp"
#include "MasterTestConstants.hpp"
#include "MasterWebModuleTestConstants.hpp"
#include "SettingLine.hpp"

using rtype::engine::settingLine;
using rtype::master::DEFAULT_WEB_FOLDER;
using rtype::master::WEB_SETTING_NAME;

/**
 * Given a site in a folder the `web` setting names
 * When the master starts and a client asks for the root of the site
 * Then the master answers the index page of that folder
 */
TEST_F(MasterModulesFixture, TheMasterServesTheSiteTheWebSettingNames) {
  writeFile(std::filesystem::path{CHOSEN_WEB_FOLDER} / SITE_INDEX_FILE,
            SITE_INDEX_TEXT);
  const auto application = masterApplication(
      {},
      {settingLine(WEB_SETTING_NAME, (folder() / CHOSEN_WEB_FOLDER).string())});

  application->start();
  const httplib::Result answer = get(SITE_ROOT_URL);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(answer->body, SITE_INDEX_TEXT);
}

/**
 * Given no `web` setting, and a site in DEFAULT_WEB_FOLDER of the folder the
 * master is started from
 * When the master starts and a client asks for the root of the site
 * Then the master answers the index page of that folder
 */
TEST_F(MasterModulesFixture, WithoutTheSettingTheMasterServesTheDefaultFolder) {
  writeFile(std::filesystem::path{DEFAULT_WEB_FOLDER} / SITE_INDEX_FILE,
            SITE_INDEX_TEXT);
  const CurrentDirectoryChange startedFrom{folder()};
  const auto application = masterApplication({}, {});

  application->start();
  const httplib::Result answer = get(SITE_ROOT_URL);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(answer->body, SITE_INDEX_TEXT);
}

/**
 * Given a site in DEFAULT_WEB_FOLDER of the folder the master is started
 * from, and the web module turned off
 * When the master starts and a client asks for the root of the site
 * Then the master answers Not Found with a problem document: it serves no site
 */
TEST_F(MasterModulesFixture, TurnedOffTheWebModuleServesNoSite) {
  writeFile(std::filesystem::path{DEFAULT_WEB_FOLDER} / SITE_INDEX_FILE,
            SITE_INDEX_TEXT);
  const CurrentDirectoryChange startedFrom{folder()};
  const auto application =
      masterApplication({}, {std::string{WEB_MODULE_OFF_LINE}});

  application->start();
  const httplib::Result answer = get(SITE_ROOT_URL);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_NOT_FOUND);
  EXPECT_EQ(answer->get_header_value(std::string{CONTENT_TYPE_HEADER}),
            EXPECTED_PROBLEM_TYPE);
}
