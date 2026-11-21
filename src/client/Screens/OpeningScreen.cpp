#include "OpeningScreen.hpp"
#include "MenuScreenIdConstants.hpp"
#include "MotionPreference.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

ScreenId openingScreen() { return HEADPHONES_SCREEN; }

ScreenId screenAfterTheHeadphones() {
  return motionPreference() == MotionPreference::Reduced ? TITLE_SCREEN
                                                         : INTRO_SCREEN;
}

}  // namespace rtype::client
