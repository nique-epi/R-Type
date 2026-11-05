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
                                               std::size_t count,
                                               std::size_t bydosInPlay) {
  std::size_t bydosDue = 0;
  for (std::size_t tick = 0; tick < count; ++tick) {
    bydosDue += director.advance(engine::SIMULATION_TICK_DURATION, bydosInPlay);
  }
  return bydosDue;
}

}  // namespace rtype::game::testing
