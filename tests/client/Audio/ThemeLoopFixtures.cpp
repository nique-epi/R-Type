#include "ThemeLoopFixtures.hpp"
#include <chrono>
#include "AudioConstants.hpp"

float themeLoopEnd() {
  return std::chrono::duration<float>(rtype::client::THEME_FIRST_BEAT).count() +
         (rtype::client::THEME_BEAT *
          static_cast<float>(rtype::client::THEME_LOOP_BEATS));
}
