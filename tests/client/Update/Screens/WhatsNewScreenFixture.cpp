#include "WhatsNewScreenFixture.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include "RecordingInterfaceCanvas.hpp"
#include "ReleaseNotes.hpp"
#include "UpdateScreenTestConstants.hpp"
#include "WhatsNewScreen.hpp"
#include "WhatsNewScreenParts.hpp"

using rtype::client::ReleaseNotes;
using rtype::client::WhatsNewScreen;
using rtype::client::WhatsNewScreenParts;
using rtype::client::testing::FIRST_NOTE;
using rtype::client::testing::NOTES_VERSION_TEXT;
using rtype::client::testing::SECOND_NOTE;

WhatsNewScreenFixture::WhatsNewScreenFixture()
    : screen_(std::make_unique<WhatsNewScreen>(
          WhatsNewScreenParts{
              .navigator = navigator_,
              .notes =
                  ReleaseNotes{.versionText = std::string{NOTES_VERSION_TEXT},
                               .lines = {std::string{FIRST_NOTE},
                                         std::string{SECOND_NOTE}}},
              .onClose = [this] { ++closes_; }},
          std::make_unique<RecordingInterfaceCanvas>(canvas_))) {}

bool WhatsNewScreenFixture::wrote(std::string_view text) const {
  return std::ranges::any_of(
      canvas_.writings,
      [text](const RecordedWriting& writing) { return writing.text == text; });
}
