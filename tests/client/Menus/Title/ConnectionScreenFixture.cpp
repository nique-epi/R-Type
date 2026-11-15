#include "ConnectionScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <string_view>
#include "ConnectionGoal.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenFixtures.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionStatus.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenInputs.hpp"
#include "ScreenTestConstants.hpp"
#include "TextMetrics.hpp"
#include "WidgetConstants.hpp"

using rtype::client::ConnectionGoal;
using rtype::client::ConnectionScreen;
using rtype::client::ConnectionScreenParts;
using rtype::client::ConnectionStatus;
using rtype::client::PRESS_DURATION;

ConnectionScreenFixture::ConnectionScreenFixture(
    ConnectionGoal goal, rtype::client::TextMetrics metrics)
    : canvas_([metrics] {
        InterfaceCanvasRecord record;
        record.metrics = metrics;
        return record;
      }()),
      choice_(launchChoice()),
      screen_(ConnectionScreenParts{.navigator = navigator_,
                                    .sounds = sounds_,
                                    .connection = connection_,
                                    .playfieldPointAt = samePlayfieldPoint,
                                    .choice = choice_,
                                    .goal = goal},
              std::make_unique<RecordingInterfaceCanvas>(canvas_)) {}

ConnectionForTheScreenBelowFixture::ConnectionForTheScreenBelowFixture()
    : ConnectionScreenFixture(ConnectionGoal::ScreenBelow) {}

const RecordingNavigator& ConnectionScreenFixture::navigator() const {
  return navigator_;
}

RecordingServerConnection& ConnectionScreenFixture::connection() {
  return connection_;
}

const InterfaceCanvasRecord& ConnectionScreenFixture::canvas() const {
  return canvas_;
}

ConnectionScreen& ConnectionScreenFixture::screen() { return screen_; }

void ConnectionScreenFixture::tap(sf::Keyboard::Key key) {
  tapKey(screen_, key);
}

void ConnectionScreenFixture::choose(sf::Keyboard::Key key) {
  tap(key);
  screen_.update(PRESS_DURATION);
}

void ConnectionScreenFixture::type(std::u32string_view text) {
  typeText(screen_, text);
}

void ConnectionScreenFixture::erase(std::size_t characters) {
  eraseCharacters(screen_, characters);
}

void ConnectionScreenFixture::show(ConnectionStatus status) {
  connection_.setStatus(status);
  screen_.update(ONE_FRAME);
}

const RecordingSoundPlayer& ConnectionScreenFixture::sounds() const {
  return sounds_;
}
