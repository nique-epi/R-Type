#include "HangarScreenFixture.hpp"
#include <SFML/Window/Event.hpp>
#include <memory>
#include "HangarScreenParts.hpp"
#include "HangarScreenTestConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ScreenTestConstants.hpp"

namespace rtype::client::testing {

HangarScreenFixture::HangarScreenFixture()
    : HangarScreenFixture(MotionPreference::Full) {}

HangarScreenFixture::HangarScreenFixture(MotionPreference motion)
    : service_(settings_, storage_),
      screen_(std::make_unique<HangarScreen>(
          HangarScreenParts{.navigator = navigator_,
                            .sounds = sounds_,
                            .settings = service_,
                            .playfieldPointAt = samePlayfieldPoint,
                            .palette = PLAYER_COLORS,
                            .shipColor = FIRST_PILOT_COLOR,
                            .motion = motion},
          std::make_unique<RecordingInterfaceCanvas>(painting_))) {}

void HangarScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(sf::Event::KeyPressed{.code = key});
  screen_->handleEvent(sf::Event::KeyReleased{.code = key});
  update(ONE_FRAME);
}

void HangarScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(sf::Event::KeyPressed{.code = key});
  update(ONE_FRAME);
}

void HangarScreenFixture::clickAt(sf::Vector2i pixel) {
  screen_->handleEvent(leftPress(pixel * INTERFACE_PIXEL));
  screen_->handleEvent(leftRelease(pixel * INTERFACE_PIXEL));
  update(ONE_FRAME);
}

void HangarScreenFixture::update(engine::Duration elapsed) {
  screen_->update(elapsed);
}

const PlayerSettings& HangarScreenFixture::settings() const {
  return settings_;
}

const RecordingSettingsStorage& HangarScreenFixture::storage() const {
  return storage_;
}

const RecordingNavigator& HangarScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& HangarScreenFixture::painting() const {
  return painting_;
}

}  // namespace rtype::client::testing
