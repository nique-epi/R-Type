#include "PlayerPreferences.hpp"
#include "FlashPreference.hpp"
#include "PlayerSettings.hpp"

namespace rtype::client {

FlashPreference flashPreferenceOf(const PlayerSettings& settings) {
  return settings.reduceFlashes ? FlashPreference::Reduced
                                : FlashPreference::Full;
}

}  // namespace rtype::client
