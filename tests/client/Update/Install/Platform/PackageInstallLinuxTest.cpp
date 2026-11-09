#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include <string>
#include "InstallFixtures.hpp"
#include "InstallPlace.hpp"
#include "InstallTestConstants.hpp"
#include "LaunchCommand.hpp"
#include "PackageInstall.hpp"

using rtype::client::installPackage;
using rtype::client::InstallPlace;
using rtype::client::LaunchCommand;
using rtype::client::testing::APPIMAGE_NAME;
using rtype::client::testing::contentOf;
using rtype::client::testing::emptyFolder;
using rtype::client::testing::INSTALL_TEST_FOLDER;
using rtype::client::testing::NEW_CONTENT;
using rtype::client::testing::OLD_CONTENT;
using rtype::client::testing::PACKAGE_NAME;
using rtype::client::testing::PARTIAL_APPIMAGE_NAME;
using rtype::client::testing::writeFile;

/**
 * Given an AppImage holding "old" and a package holding "new"
 * When the package is installed
 * Then the AppImage holds "new", may run, no partial file is left, and the
 * command starts the AppImage
 */
TEST(PackageInstallLinux, PutsThePackageInPlaceOfTheAppImageAndStartsIt) {
#ifndef __linux__
  GTEST_SKIP() << "the AppImage is installed on Linux only";
#else
  const std::filesystem::path folder = emptyFolder(INSTALL_TEST_FOLDER);
  writeFile(folder / APPIMAGE_NAME, OLD_CONTENT);
  writeFile(folder / PACKAGE_NAME, NEW_CONTENT);
  const InstallPlace place{.installed = folder / APPIMAGE_NAME,
                           .system = "Linux"};

  const std::optional<LaunchCommand> command =
      installPackage(folder / PACKAGE_NAME, place);

  ASSERT_TRUE(command.has_value());
  EXPECT_EQ(command.value_or(LaunchCommand{}).program, place.installed);
  EXPECT_EQ(contentOf(place.installed), NEW_CONTENT);
  EXPECT_NE(std::filesystem::status(place.installed).permissions() &
                std::filesystem::perms::owner_exec,
            std::filesystem::perms::none);
  EXPECT_FALSE(std::filesystem::exists(folder / PARTIAL_APPIMAGE_NAME));
#endif
}
