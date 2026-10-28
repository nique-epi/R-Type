#include "PlayerPreferencesFixtures.hpp"
#include "ColorMode.hpp"
#include "PlayerSettings.hpp"

rtype::client::PlayerSettings settingsFor(rtype::client::ColorMode mode) {
  rtype::client::PlayerSettings settings;
  settings.colorMode = mode;
  return settings;
}
