#include "SkyEventSchedulerFixtures.hpp"
#include <cmath>
#include "SkyEventScheduler.hpp"
#include "SkyEventTestConstants.hpp"

using rtype::client::SkyEventScheduler;

int framesIn(float seconds) {
  return static_cast<int>(std::lround(seconds / FRAME));
}

void advanceFor(SkyEventScheduler& scheduler, float seconds) {
  for (int frame = 0; frame < framesIn(seconds); ++frame) {
    scheduler.advance(FRAME);
  }
}
