#include "LargestEndScreenFixture.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include "EndScreenFixture.hpp"
#include "EndScreenTestConstants.hpp"
#include "GameEnded.hpp"
#include "InterfaceConstants.hpp"
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
    for (const RecordedWriting& writing : canvas().writings) {
      const sf::IntRect box = writtenBoxOf(canvas(), writing);
      if (!writing.text.empty() &&
          (box.position.x < 0 || box.position.x + box.size.x >
                                     rtype::client::INTERFACE_AREA.size.x)) {
        off.push_back(writing.text);
      }
    }
  }
  return off;
}
