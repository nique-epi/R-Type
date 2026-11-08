#include "LobbyRulesFixture.hpp"
#include <memory>
#include <optional>
#include <utility>
#include "ConnectionPhase.hpp"
#include "GameRules.hpp"
#include "InterfaceColorConstants.hpp"
#include "LobbyPlace.hpp"
#include "LobbyScreen.hpp"
#include "LobbyScreenParts.hpp"
#include "LobbyScreenTestConstants.hpp"
#include "MotionPreference.hpp"
#include "OptionsScreenFixtures.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::ConnectionPhase;
using rtype::client::LobbyPlace;
using rtype::client::LobbyScreen;
using rtype::client::LobbyScreenParts;
using rtype::client::MotionPreference;
using rtype::client::PLAYER_COLORS;
using rtype::client::testing::OWN_NUMBER;
using rtype::network::GameRules;

void LobbyRulesFixture::open(std::optional<GameRules> rules) {
  connection_.setStatus(
      {.phase = ConnectionPhase::Accepted, .playerNumber = OWN_NUMBER});
  screen_ = std::make_unique<LobbyScreen>(
      LobbyScreenParts{.navigator = navigator_,
                       .sounds = sounds_,
                       .connection = connection_,
                       .waitingRoom = waitingRoom_,
                       .playfieldPointAt = samePlayfieldPoint,
                       .keys = settings_.keys,
                       .keyName = nameInTheFile,
                       .palette = PLAYER_COLORS,
                       .look = look_,
                       .motion = MotionPreference::Full,
                       .place = LobbyPlace::AfterConnection,
                       .chat = chat_,
                       .settings = settings_,
                       .rules = std::move(rules)},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
  screen_->update(ONE_FRAME);
}

const InterfaceCanvasRecord& LobbyRulesFixture::canvas() const {
  return canvas_;
}
