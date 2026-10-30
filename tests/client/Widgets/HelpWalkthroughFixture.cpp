#include "HelpWalkthroughFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include "BoundAction.hpp"
#include "HelpWalkthrough.hpp"
#include "HelpWalkthroughParts.hpp"
#include "HelpWalkthroughTestConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "OptionsScreenFixtures.hpp"
#include "PointerEvents.hpp"
#include "ProfessorSpriteConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingSoundPlayer.hpp"

using rtype::client::BoundAction;
using rtype::client::HelpWalkthrough;
using rtype::client::HelpWalkthroughParts;
using rtype::client::MotionPreference;
using rtype::client::PLAYER_COLORS;
using rtype::client::PROFESSOR_SHEET_ID;
using rtype::client::testing::HELP_CHIP;
using rtype::client::testing::ROOM_AREAS;

HelpWalkthroughFixture::HelpWalkthroughFixture() {
  rebuild(MotionPreference::Full);
}

void HelpWalkthroughFixture::rebuild(MotionPreference motion) {
  help_ = std::make_unique<HelpWalkthrough>(
      HelpWalkthroughParts{.sounds = sounds_,
                           .keys = keys_,
                           .keyName = nameInTheFile,
                           .palette = PLAYER_COLORS,
                           .motion = motion,
                           .chipArea = HELP_CHIP});
}

void HelpWalkthroughFixture::bind(BoundAction action,
                                  sf::Keyboard::Scancode key) {
  keys_.bind(action, key);
  rebuild(MotionPreference::Full);
}

bool HelpWalkthroughFixture::press(sf::Keyboard::Key key) {
  return help().handleEvent(keyPress(key), samePlayfieldPoint);
}

bool HelpWalkthroughFixture::click(sf::Vector2i windowPixel) {
  return help().handleEvent(leftPress(windowPixel), samePlayfieldPoint);
}

void HelpWalkthroughFixture::showStep(std::size_t step) {
  press(sf::Keyboard::Key::H);
  for (std::size_t shown = 0; shown < step; ++shown) {
    press(sf::Keyboard::Key::Enter);
  }
}

const InterfaceCanvasRecord& HelpWalkthroughFixture::painted() {
  canvas_.clear();
  help().paint(canvas_, ROOM_AREAS);
  return record_;
}

std::optional<RecordedImage> HelpWalkthroughFixture::professorImage() {
  const InterfaceCanvasRecord& record = painted();
  const auto professor = std::ranges::find(record.images, PROFESSOR_SHEET_ID,
                                           &RecordedImage::assetId);
  if (professor == record.images.end()) {
    return std::nullopt;
  }
  return *professor;
}

HelpWalkthrough& HelpWalkthroughFixture::help() { return *help_; }

const RecordingSoundPlayer& HelpWalkthroughFixture::sounds() const {
  return sounds_;
}
