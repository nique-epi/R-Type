#include <gtest/gtest.h>
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "SkyLight.hpp"

using rtype::client::PlayerSettings;
using rtype::client::SkyLight;
using rtype::client::skyLightOf;

/**
 * Given settings where the player turned REDUCE FLASHES on
 * When the light of the sky is asked for
 * Then the sky shines steadily
 */
TEST(PlayerPreferences, ReducedFlashesMakeTheSkySteady) {
  PlayerSettings settings;
  settings.reduceFlashes = true;

  EXPECT_EQ(skyLightOf(settings), SkyLight::Steady);
}

/**
 * Given settings where REDUCE FLASHES is off
 * When the light of the sky is asked for
 * Then the sky twinkles
 */
TEST(PlayerPreferences, FullFlashesLetTheSkyTwinkle) {
  PlayerSettings settings;
  settings.reduceFlashes = false;

  EXPECT_EQ(skyLightOf(settings), SkyLight::Twinkling);
}
