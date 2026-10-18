#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include "SettingsFolder.hpp"

using rtype::client::settingsFolder;

/**
 * Given the system the tests run on
 * When the settings folder is asked for
 * Then it is an absolute path to a folder named after the game
 */
TEST(SettingsFolderTest, TheFolderIsAbsoluteAndNamedAfterTheGame) {
  const std::filesystem::path folder =
      settingsFolder().value_or(std::filesystem::path());
  EXPECT_TRUE(folder.is_absolute());
  const std::string name = folder.filename().string();
  EXPECT_TRUE(name == "r-type" || name == "R-Type") << name;
}

#ifdef __linux__

namespace {

/** @brief Sets the two variables the Linux folder is read from for one test,
 * then puts back what they held. */
class LinuxSettingsFolderTest : public ::testing::Test {
 protected:
  void SetUp() override {
    configuration_ = read("XDG_CONFIG_HOME");
    home_ = read("HOME");
  }

  void TearDown() override {
    restore("XDG_CONFIG_HOME", configuration_);
    restore("HOME", home_);
  }

  static void set(const char* variable, const char* value) {
    // NOLINTNEXTLINE(concurrency-mt-unsafe,misc-include-cleaner)
    ::setenv(variable, value, 1);
  }

  static void unset(const char* variable) {
    // NOLINTNEXTLINE(concurrency-mt-unsafe,misc-include-cleaner)
    ::unsetenv(variable);
  }

 private:
  static std::optional<std::string> read(const char* variable) {
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    const char* const value = std::getenv(variable);
    return value == nullptr ? std::nullopt : std::optional<std::string>(value);
  }

  static void restore(const char* variable,
                      const std::optional<std::string>& value) {
    if (value.has_value()) {
      set(variable, value->c_str());
    } else {
      unset(variable);
    }
  }

  std::optional<std::string> configuration_;
  std::optional<std::string> home_;
};

}  // namespace

/**
 * Given XDG_CONFIG_HOME set to an absolute folder
 * When the settings folder is asked for
 * Then it is the r-type folder in it
 */
TEST_F(LinuxSettingsFolderTest, XdgConfigHomeHoldsTheFolder) {
  set("XDG_CONFIG_HOME", "/tmp/xdg");
  EXPECT_EQ(settingsFolder(), std::filesystem::path("/tmp/xdg/r-type"));
}

/**
 * Given XDG_CONFIG_HOME relative, and a home
 * When the settings folder is asked for
 * Then the relative value is ignored, as the XDG specification asks, and
 * the folder is in .config of the home
 */
TEST_F(LinuxSettingsFolderTest, ARelativeXdgConfigHomeIsIgnored) {
  set("XDG_CONFIG_HOME", "relative");
  set("HOME", "/home/pilot");
  EXPECT_EQ(settingsFolder(),
            std::filesystem::path("/home/pilot/.config/r-type"));
}

/**
 * Given XDG_CONFIG_HOME empty, and a home
 * When the settings folder is asked for
 * Then the folder is in .config of the home
 */
TEST_F(LinuxSettingsFolderTest, AnEmptyXdgConfigHomeFallsBackToTheHome) {
  set("XDG_CONFIG_HOME", "");
  set("HOME", "/home/pilot");
  EXPECT_EQ(settingsFolder(),
            std::filesystem::path("/home/pilot/.config/r-type"));
}

/**
 * Given neither XDG_CONFIG_HOME nor HOME
 * When the settings folder is asked for
 * Then there is none
 */
TEST_F(LinuxSettingsFolderTest, NoVariableGivesNoFolder) {
  unset("XDG_CONFIG_HOME");
  unset("HOME");
  EXPECT_FALSE(settingsFolder().has_value());
}

#endif
