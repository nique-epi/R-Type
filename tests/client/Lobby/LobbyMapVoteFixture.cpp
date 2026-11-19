#include "LobbyMapVoteFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ConnectionPhase.hpp"
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
#include "ScreenTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::ConnectionPhase;
using rtype::client::LobbyPlace;
using rtype::client::LobbyScreen;
using rtype::client::LobbyScreenParts;
using rtype::client::MotionPreference;
using rtype::client::PLAYER_COLORS;
using rtype::client::PRESS_DURATION;
using rtype::client::testing::MAP_KEY;
using rtype::client::testing::OWN_NUMBER;
using rtype::network::LobbyPlayer;

void LobbyMapVoteFixture::open(std::vector<LobbyPlayer> players,
                               std::vector<std::string> maps) {
  connection_.setStatus(
      {.phase = ConnectionPhase::Accepted, .playerNumber = OWN_NUMBER});
  waitingRoom_.setLobbyPlayers(std::move(players));
  waitingRoom_.setLobbyMaps(std::move(maps));
  screen_ = std::make_unique<LobbyScreen>(
      LobbyScreenParts{.navigator = navigator_,
                       .sounds = sounds_,
                       .connection = connection_,
                       .waitingRoom = waitingRoom_,
                       .shipLooks = shipLooks_,
                       .mapBallot = mapBallot_,
                       .playfieldPointAt = samePlayfieldPoint,
                       .keys = settings_.keys,
                       .keyName = nameInTheFile,
                       .palette = PLAYER_COLORS,
                       .look = look_,
                       .motion = MotionPreference::Full,
                       .place = LobbyPlace::AfterConnection,
                       .chat = chat_,
                       .settings = settings_,
                       .rules = std::nullopt},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
  screen_->update(ONE_FRAME);
}

void LobbyMapVoteFixture::pressMapKey(bool held) {
  screen_->handleEvent(keyPress(sf::Keyboard::Key::M, MAP_KEY));
  if (!held) {
    screen_->handleEvent(keyRelease(sf::Keyboard::Key::M, MAP_KEY));
  }
  screen_->update(ONE_FRAME);
}

void LobbyMapVoteFixture::click(sf::Vector2i pixel) {
  screen_->handleEvent(leftPress(pixel));
  screen_->handleEvent(leftRelease(pixel));
  screen_->update(PRESS_DURATION);
}

const std::vector<std::uint8_t>& LobbyMapVoteFixture::votes() const {
  return mapBallot_.votes();
}

const InterfaceCanvasRecord& LobbyMapVoteFixture::canvas() const {
  return canvas_;
}
