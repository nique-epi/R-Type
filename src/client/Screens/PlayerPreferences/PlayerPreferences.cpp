#include "PlayerPreferences.hpp"
#include "FlashPreference.hpp"
#include "PlayerSettings.hpp"
#include "SkyLight.hpp"

namespace rtype::client {

FlashPreference flashPreferenceOf(const PlayerSettings& settings) {
  return settings.reduceFlashes ? FlashPreference::Reduced
                                : FlashPreference::Full;
}

SkyLight skyLightOf(const PlayerSettings& settings) {
  return settings.reduceFlashes ? SkyLight::Steady : SkyLight::Twinkling;
}

}  // namespace rtype::client
