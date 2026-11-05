#include "WaveDirectorLevelFixture.hpp"
#include <cstddef>
#include "TimeConstants.hpp"
#include "WaveDirector.hpp"
#include "WaveTestConstants.hpp"

namespace rtype::game::testing {

WaveDirectorLevelFixture::WaveDirectorLevelFixture()
    : oneWaveLevel_(ONE_WAVE_LEVEL_CURVE),
      twoWaveLevel_(TWO_WAVE_LEVEL_CURVE) {}

WaveDirector& WaveDirectorLevelFixture::oneWaveLevel() { return oneWaveLevel_; }

WaveDirector& WaveDirectorLevelFixture::twoWaveLevel() { return twoWaveLevel_; }

std::size_t WaveDirectorLevelFixture::runTicks(WaveDirector& director,
                                               LevelTicks ticks) {
  std::size_t enemiesDue = 0;
  for (std::size_t tick = 0; tick < ticks.count; ++tick) {
    enemiesDue +=
        director.advance(engine::SIMULATION_TICK_DURATION, ticks.enemiesInPlay);
  }
  return enemiesDue;
}

}  // namespace rtype::game::testing
