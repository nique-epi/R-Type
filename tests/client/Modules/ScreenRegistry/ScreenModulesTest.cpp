#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ClientException.hpp"
#include "ClientGameModule.hpp"
#include "ClientLobbyModule.hpp"
#include "ClientMenusModule.hpp"
#include "GameScreenIdConstants.hpp"
#include "IModule.hpp"
#include "ModuleConstants.hpp"
#include "ScreenId.hpp"
#include "ScreenModulesFixture.hpp"
#include "ScreenModulesTestConstants.hpp"

using rtype::client::ClientGameModule;
using rtype::client::ClientLobbyModule;
using rtype::client::ClientMenusModule;
using rtype::client::DuplicateScreenException;
using rtype::client::GAME_SCREEN;
using rtype::client::ScreenId;
using rtype::client::SCREENS_MODULE_NAME;

/**
 * Given the double of the screens module and the menus module installed
 * When each screen of the menus is looked for in the registry
 * Then every one of them is registered
 */
TEST_F(ScreenModulesFixture, MenusModuleRegistersItsScreens) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientMenusModule>());
  install(std::move(modules));

  for (const ScreenId screen : MENU_SCREENS) {
    SCOPED_TRACE(screen.name());
    EXPECT_TRUE(isRegistered(screen));
  }
}

/**
 * Given the double of the screens module and the lobby module installed
 * When each screen of the waiting room is looked for in the registry
 * Then every one of them is registered
 */
TEST_F(ScreenModulesFixture, LobbyModuleRegistersItsScreens) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientLobbyModule>());
  install(std::move(modules));

  for (const ScreenId screen : LOBBY_SCREENS) {
    SCOPED_TRACE(screen.name());
    EXPECT_TRUE(isRegistered(screen));
  }
}

/**
 * Given the double of the screens module and the game module installed
 * When each screen of a game is looked for in the registry
 * Then every one of them is registered
 */
TEST_F(ScreenModulesFixture, GameModuleRegistersItsScreens) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientGameModule>());
  install(std::move(modules));

  for (const ScreenId screen : GAME_SCREENS) {
    SCOPED_TRACE(screen.name());
    EXPECT_TRUE(isRegistered(screen));
  }
}

/**
 * Given the double of the screens module and only the lobby module installed
 * When a screen of the game module is looked for
 * Then it is not registered: a module registers its own screens only
 */
TEST_F(ScreenModulesFixture, ModuleDoesNotRegisterTheScreensOfAnother) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientLobbyModule>());
  install(std::move(modules));

  EXPECT_FALSE(isRegistered(GAME_SCREEN));
}

/**
 * Given the menus module listed twice
 * When the modules are installed
 * Then the installation stops on the first screen registered twice
 */
TEST_F(ScreenModulesFixture,
       TwoModulesRegisteringOneScreenStopTheInstallation) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientMenusModule>());
  modules.push_back(std::make_unique<ClientMenusModule>());

  EXPECT_THROW(install(std::move(modules)), DuplicateScreenException);
}

/**
 * Given the menus module
 * When its dependencies are read
 * Then they come after the screens module, whose registry they fill
 */
TEST_F(ScreenModulesFixture, MenusModuleDependsOnTheScreensModule) {
  const std::vector<std::string> dependencies =
      ClientMenusModule{}.dependencies();

  EXPECT_EQ(dependencies,
            (std::vector<std::string>{std::string{SCREENS_MODULE_NAME}}));
}

/**
 * Given the lobby module
 * When its dependencies are read
 * Then it comes after the screens module, whose registry it fills
 */
TEST_F(ScreenModulesFixture, LobbyModuleDependsOnTheScreensModule) {
  const std::vector<std::string> dependencies =
      ClientLobbyModule{}.dependencies();

  EXPECT_EQ(dependencies,
            (std::vector<std::string>{std::string{SCREENS_MODULE_NAME}}));
}

/**
 * Given the game module
 * When its dependencies are read
 * Then it comes after the screens module, whose registry it fills
 */
TEST_F(ScreenModulesFixture, GameModuleDependsOnTheScreensModule) {
  const std::vector<std::string> dependencies =
      ClientGameModule{}.dependencies();

  EXPECT_EQ(dependencies,
            (std::vector<std::string>{std::string{SCREENS_MODULE_NAME}}));
}
