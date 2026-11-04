#include "BotInputSenderFixture.hpp"
#include <cstddef>
#include "RecordingPlayerInputLink.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

void BotInputSenderFixture::setGameStarted(bool started) {
  room_.setGameStarted(started);
}

std::size_t BotInputSenderFixture::advanceFor(std::size_t frames,
                                              engine::Duration frame) {
  std::size_t sent = 0;
  for (std::size_t count = 0; count < frames; ++count) {
    if (sender_.advance(frame)) {
      ++sent;
    }
  }
  return sent;
}

const RecordingPlayerInputLink& BotInputSenderFixture::link() const {
  return link_;
}

}  // namespace rtype::tools::testing
