#include "SpokenLineFixture.hpp"
#include <memory>
#include <span>
#include <string>
#include <vector>
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "SpokenLine.hpp"
#include "SpokenLineParts.hpp"
#include "SpokenLineTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

SpokenLineFixture::SpokenLineFixture() { build(MotionPreference::Full, true); }

void SpokenLineFixture::rebuildWithLessMotion() {
  build(MotionPreference::Reduced, true);
}

void SpokenLineFixture::rebuildToStay() {
  SpokenLineParts parts = partsWith(MotionPreference::Full, true);
  parts.stays = true;
  line_ = std::make_unique<SpokenLine>(parts);
}

void SpokenLineFixture::rebuildWithoutVoice() {
  build(MotionPreference::Full, false);
}

SpokenLine& SpokenLineFixture::line() { return *line_; }

void SpokenLineFixture::walk(engine::Duration elapsed) {
  for (engine::Duration walked{}; walked < elapsed; walked += SHORT_FRAME) {
    line_->advance(SHORT_FRAME);
  }
}

std::vector<std::string> SpokenLineFixture::written() {
  painting_ = {};
  RecordingInterfaceCanvas canvas(painting_);
  line_->paint(canvas, TEST_BUBBLE_BOTTOM_LEFT);
  std::vector<std::string> texts;
  texts.reserve(painting_.writings.size());
  for (const RecordedWriting& writing : painting_.writings) {
    texts.push_back(writing.text);
  }
  return texts;
}

const InterfaceCanvasRecord& SpokenLineFixture::painting() const {
  return painting_;
}

const std::vector<std::string>& SpokenLineFixture::played() const {
  return sounds_.played();
}

void SpokenLineFixture::build(MotionPreference motion, bool voiced) {
  line_ = std::make_unique<SpokenLine>(partsWith(motion, voiced));
}

SpokenLineParts SpokenLineFixture::partsWith(MotionPreference motion,
                                             bool voiced) {
  return SpokenLineParts{.sounds = sounds_,
                         .voice = voiced
                                      ? std::span<const char* const>(TEST_VOICE)
                                      : std::span<const char* const>(),
                         .width = TEST_BUBBLE_WIDTH,
                         .tail = TEST_TAIL,
                         .motion = motion,
                         .stays = false};
}

}  // namespace rtype::client::testing
