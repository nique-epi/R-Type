#include <gtest/gtest.h>
#include "Application.hpp"
#include "BuiltModuleConstants.hpp"
#include "ClientModulesFixture.hpp"
#include "ModuleConstants.hpp"

using rtype::client::AUDIO_MODULE_NAME;
using rtype::client::METRICS_MODULE_NAME;
using rtype::client::SCREENS_MODULE_NAME;
using rtype::engine::Application;
using rtype::engine::AUDIO_MODULE_BUILT;
using rtype::engine::METRICS_MODULE_BUILT;

/**
 * Given the modules of the client, in a build that compiled the sound
 * When the place of the sound and of the screens is read
 * Then the sound comes first: the screens find its service only when it is
 * installed before them
 */
TEST_F(ClientModulesFixture, TheSoundIsListedBeforeTheScreens) {
  if (!AUDIO_MODULE_BUILT) {
    GTEST_SKIP();
  }
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

/**
 * Given the modules of the client, in a build with or without the sound
 * When they are listed
 * Then the sound is among them exactly when the build compiled it
 */
TEST_F(ClientModulesFixture, ListsTheSoundOnlyWhenItIsBuilt) {
  EXPECT_EQ(lists(AUDIO_MODULE_NAME), AUDIO_MODULE_BUILT);
}

/**
 * Given the modules of the client, in a build with or without the metrics
 * When they are listed
 * Then the metrics are among them exactly when the build compiled them
 */
TEST_F(ClientModulesFixture, ListsTheMetricsOnlyWhenTheyAreBuilt) {
  EXPECT_EQ(lists(METRICS_MODULE_NAME), METRICS_MODULE_BUILT);
}
