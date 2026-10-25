#include "LobbyScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>
#include "ConnectionStatus.hpp"
#include "KeyPress.hpp"
#include "LobbyScreen.hpp"
#include "LobbyScreenParts.hpp"
#include "LobbyScreenTestConstants.hpp"
#include "LobbyState.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::ConnectionStatus;
using rtype::client::LobbyScreen;
using rtype::client::LobbyScreenParts;
using rtype::client::MotionPreference;
using rtype::client::testing::OWN_NUMBER;
using rtype::client::testing::READY_KEY;
using rtype::client::testing::READY_KEY_NAME;
using rtype::network::LobbyPlayer;

LobbyScreenFixture::LobbyScreenFixture()
    : screen_(LobbyScreenParts{.navigator = navigator_,
                               .connection = connection_,
                               .playfieldPointAt = samePlayfieldPoint,
                               .readyKey = {.key = READY_KEY,
                                            .name = READY_KEY_NAME},
                               .motion = MotionPreference::Full},
              std::make_unique<RecordingInterfaceCanvas>(canvas_)) {
  connection_.setStatus(
      {.phase = ConnectionStatus::Phase::Accepted, .playerNumber = OWN_NUMBER});
}

void LobbyScreenFixture::list(std::vector<LobbyPlayer> players) {
  connection_.setLobbyPlayers(std::move(players));
  screen_.update(ONE_FRAME);
}

void LobbyScreenFixture::tap(sf::Keyboard::Key key,
                             sf::Keyboard::Scancode scancode) {
  screen_.handleEvent(keyPress(key, scancode));
  screen_.handleEvent(keyRelease(key, scancode));
}

std::vector<ServerConnectionCall> LobbyScreenFixture::readyRequests() const {
  std::vector<ServerConnectionCall> requests;
  std::ranges::copy_if(connection_.calls(), std::back_inserter(requests),
                       [](const ServerConnectionCall& call) noexcept {
                         return call.request ==
                                ServerConnectionCall::Request::SetReady;
                       });
  return requests;
}

const RecordingNavigator& LobbyScreenFixture::navigator() const {
  return navigator_;
}

RecordingServerConnection& LobbyScreenFixture::connection() {
  return connection_;
}

const InterfaceCanvasRecord& LobbyScreenFixture::canvas() const {
  return canvas_;
}

LobbyScreen& LobbyScreenFixture::screen() { return screen_; }
