#include "WaveDirectorFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include "TimeConstants.hpp"
#include "WaveDirector.hpp"
#include "WaveDirectorTestConstants.hpp"
#include "WaveTestConstants.hpp"

namespace rtype::game::testing {

WaveDirectorFixture::WaveDirectorFixture() : director_(TEST_WAVE_CURVE) {}

std::size_t WaveDirectorFixture::runTicksWithABydoInPlay(std::size_t count) {
  std::size_t bydosDue = 0;
  for (std::size_t tick = 0; tick < count; ++tick) {
    bydosDue +=
        director_.advance(engine::SIMULATION_TICK_DURATION, ONE_BYDO_IN_PLAY);
  }
  return bydosDue;
}

void WaveDirectorFixture::runATickWithTheSkyClear() {
  static_cast<void>(
      director_.advance(engine::SIMULATION_TICK_DURATION, NO_BYDO_IN_PLAY));
}

std::size_t WaveDirectorFixture::ticksToTheNextBydo(std::size_t bydosInPlay) {
  for (std::size_t tick = 1; tick <= A_MINUTE_OF_TICKS; ++tick) {
    if (director_.advance(engine::SIMULATION_TICK_DURATION, bydosInPlay) > 0) {
      return tick;
    }
  }
  ADD_FAILURE() << "no Bydo was due within a minute";
  return 0;
}

void WaveDirectorFixture::letTheFirstWaveAppear() {
  static_cast<void>(runTicksWithABydoInPlay(TICKS_TO_THE_WHOLE_FIRST_WAVE));
}

const WaveDirector& WaveDirectorFixture::director() const { return director_; }

}  // namespace rtype::game::testing
