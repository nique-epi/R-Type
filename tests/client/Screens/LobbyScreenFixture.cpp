#include "LobbyScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <utility>
#include <vector>
#include "ConnectionStatus.hpp"
#include "InterfaceColorConstants.hpp"
#include "KeyPress.hpp"
#include "LobbyPlace.hpp"
#include "LobbyScreen.hpp"
#include "LobbyScreenParts.hpp"
#include "LobbyScreenTestConstants.hpp"
#include "LobbyState.hpp"
#include "MotionPreference.hpp"
#include "OptionsScreenFixtures.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "RecordingWaitingRoom.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::ConnectionStatus;
using rtype::client::LobbyPlace;
using rtype::client::LobbyScreen;
using rtype::client::LobbyScreenParts;
using rtype::client::MotionPreference;
using rtype::client::PLAYER_COLORS;
using rtype::client::testing::OWN_NUMBER;
using rtype::network::LobbyPlayer;

LobbyScreenFixture::LobbyScreenFixture(LobbyPlace place)
    : screen_(LobbyScreenParts{.navigator = navigator_,
                               .sounds = sounds_,
                               .connection = connection_,
                               .waitingRoom = waitingRoom_,
                               .playfieldPointAt = samePlayfieldPoint,
                               .keys = keys_,
                               .keyName = nameInTheFile,
                               .palette = PLAYER_COLORS,
                               .motion = MotionPreference::Full,
                               .place = place},
              std::make_unique<RecordingInterfaceCanvas>(canvas_)) {
  connection_.setStatus(
      {.phase = ConnectionStatus::Phase::Accepted, .playerNumber = OWN_NUMBER});
}

void LobbyScreenFixture::list(std::vector<LobbyPlayer> players) {
  waitingRoom_.setLobbyPlayers(std::move(players));
  screen_.update(ONE_FRAME);
}

void LobbyScreenFixture::tap(sf::Keyboard::Key key,
                             sf::Keyboard::Scancode scancode) {
  screen_.handleEvent(keyPress(key, scancode));
  screen_.handleEvent(keyRelease(key, scancode));
}

const std::vector<bool>& LobbyScreenFixture::readyRequests() const {
  return waitingRoom_.readyRequests();
}

const RecordingNavigator& LobbyScreenFixture::navigator() const {
  return navigator_;
}

RecordingServerConnection& LobbyScreenFixture::connection() {
  return connection_;
}

RecordingWaitingRoom& LobbyScreenFixture::waitingRoom() { return waitingRoom_; }

const InterfaceCanvasRecord& LobbyScreenFixture::canvas() const {
  return canvas_;
}

LobbyScreen& LobbyScreenFixture::screen() { return screen_; }

const RecordingSoundPlayer& LobbyScreenFixture::sounds() const {
  return sounds_;
}
