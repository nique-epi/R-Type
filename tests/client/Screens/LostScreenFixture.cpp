#include "LostScreenFixture.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <memory>
#include "ConnectionPhase.hpp"
#include "FlashPreference.hpp"
#include "LostScreen.hpp"
#include "LostScreenParts.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"

using rtype::client::ConnectionPhase;
using rtype::client::FlashPreference;
using rtype::client::LostScreen;
using rtype::client::LostScreenParts;
using rtype::client::MotionPreference;

LostScreenFixture::LostScreenFixture() {
  rebuildFor(ConnectionPhase::Lost, MotionPreference::Full);
}

void LostScreenFixture::rebuildFor(ConnectionPhase ending,
                                   MotionPreference motion) {
  screen_ = std::make_unique<LostScreen>(
      LostScreenParts{.navigator = navigator_,
                      .sounds = sounds_,
                      .picture = sf::Texture{},
                      .ending = ending,
                      .playfieldPointAt = samePlayfieldPoint,
                      .motion = motion,
                      .flashes = FlashPreference::Full},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

LostScreen& LostScreenFixture::screen() { return *screen_; }

const RecordingNavigator& LostScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& LostScreenFixture::canvas() const {
  return canvas_;
}

const RecordingSoundPlayer& LostScreenFixture::sounds() const {
  return sounds_;
}
