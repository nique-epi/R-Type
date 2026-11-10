#include "UpdateScreenFixture.hpp"
#include <algorithm>
#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include "FakeUpdateRun.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "UpdateScreen.hpp"
#include "UpdateScreenParts.hpp"
#include "UpdateScreenTestConstants.hpp"

using rtype::client::UpdateScreen;
using rtype::client::UpdateScreenParts;
using rtype::client::testing::UPDATE_FRAME;
using rtype::client::testing::VERSION_LINE;

UpdateScreenFixture::UpdateScreenFixture()
    : screen_(std::make_unique<UpdateScreen>(
          UpdateScreenParts{.navigator = navigator_,
                            .run = run_,
                            .versionLine = std::string{VERSION_LINE}},
          std::make_unique<RecordingInterfaceCanvas>(canvas_))) {}

void UpdateScreenFixture::runFor(std::chrono::milliseconds total) {
  for (std::chrono::milliseconds spent{}; spent < total;
       spent += UPDATE_FRAME) {
    screen_->update(UPDATE_FRAME);
  }
}

bool UpdateScreenFixture::wrote(std::string_view text) const {
  return std::ranges::any_of(
      canvas_.writings,
      [text](const RecordedWriting& writing) { return writing.text == text; });
}

const RecordingNavigator& UpdateScreenFixture::navigator() const {
  return navigator_;
}

FakeUpdateRun& UpdateScreenFixture::run() { return run_; }

rtype::client::UpdateScreen& UpdateScreenFixture::screen() { return *screen_; }
