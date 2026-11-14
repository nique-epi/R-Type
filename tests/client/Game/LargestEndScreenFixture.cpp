#include "LargestEndScreenFixture.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include "EndScreenFixture.hpp"
#include "EndScreenTestConstants.hpp"
#include "GameEnded.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

LargestEndScreenFixture::LargestEndScreenFixture()
    : EndScreenFixture(rtype::network::WireGameOutcome::Defeat,
                       rtype::client::testing::LARGEST_END_TEXT) {}

std::vector<std::string> LargestEndScreenFixture::writingsOutSidewaysFor(
    rtype::engine::Duration duration) {
  std::vector<std::string> off;
  while (duration > rtype::engine::Duration::zero()) {
    const rtype::engine::Duration step = std::min(duration, ONE_FRAME);
    advance(step);
    duration -= step;
    const std::vector<std::string> now = writingsOutSideways(canvas());
    off.insert(off.end(), now.begin(), now.end());
  }
  return off;
}
