#include "PlayerPreferences.hpp"
#include "ColorMode.hpp"
#include "FlashPreference.hpp"
#include "PilotShadeConstants.hpp"
#include "PilotShades.hpp"
#include "PlayerPalette.hpp"
#include "PlayerSettings.hpp"

namespace rtype::client {

FlashPreference flashPreferenceOf(const PlayerSettings& settings) {
  return settings.reduceFlashes ? FlashPreference::Reduced
                                : FlashPreference::Full;
}

PilotShades pilotShadesOf(const PlayerSettings& settings) {
  switch (settings.colorMode) {
    case ColorMode::Protanopia:
      return PROTANOPIA_PILOT_SHADES;
    case ColorMode::Deuteranopia:
      return DEUTERANOPIA_PILOT_SHADES;
    case ColorMode::Tritanopia:
      return TRITANOPIA_PILOT_SHADES;
    case ColorMode::Off:
      break;
  }
  return STANDARD_PILOT_SHADES;
}

const PlayerPalette& playerPaletteOf(const PlayerSettings& settings) {
  return pilotShadesOf(settings).palette.get();
}

}  // namespace rtype::client
