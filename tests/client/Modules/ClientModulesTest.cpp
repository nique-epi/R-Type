#include <gtest/gtest.h>
#include "Application.hpp"
#include "ClientModulesFixture.hpp"
#include "ModuleConstants.hpp"

using rtype::client::AUDIO_MODULE_NAME;
using rtype::client::SCREENS_MODULE_NAME;
using rtype::engine::Application;

/**
 * Given the modules of the client
 * When the place of the sound and of the screens is read
 * Then the sound comes first: the screens find its service only when it is
 * installed before them
 */
TEST_F(ClientModulesFixture, TheSoundIsListedBeforeTheScreens) {
  EXPECT_LT(positionOf(AUDIO_MODULE_NAME), positionOf(SCREENS_MODULE_NAME));
}

/**
 * Given the modules of the client without the sound module
 * When the application is built from them
 * Then it is accepted: no module depends on the sound
 */
TEST_F(ClientModulesFixture, AListWithoutTheSoundModuleIsAccepted) {
  EXPECT_NO_THROW(
      const Application application{modulesWithout(AUDIO_MODULE_NAME)});
}
