#include "LobbyMapVoteFixture.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ConnectionPhase.hpp"
#include "InterfaceColorConstants.hpp"
#include "KeyPress.hpp"
#include "LobbyMap.hpp"
#include "LobbyPlace.hpp"
#include "LobbyScreen.hpp"
#include "LobbyScreenConstants.hpp"
#include "LobbyScreenParts.hpp"
#include "LobbyScreenTestConstants.hpp"
#include "LobbyState.hpp"
#include "MotionPreference.hpp"
#include "OptionsScreenFixtures.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingMapPreviews.hpp"
#include "ScreenTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::ConnectionPhase;
using rtype::client::LobbyPlace;
using rtype::client::LobbyScreen;
using rtype::client::LobbyScreenParts;
using rtype::client::MAP_PREVIEW_SIZE;
using rtype::client::MotionPreference;
using rtype::client::PLAYER_COLORS;
using rtype::client::PRESS_DURATION;
using rtype::client::testing::AZERTY_M_PLACE;
using rtype::client::testing::FIRST_MAP_PREVIEW;
using rtype::client::testing::FIRST_VOTED_MAP;
using rtype::client::testing::MAP_KEY_CODE;
using rtype::client::testing::OWN_NUMBER;
using rtype::client::testing::SECOND_MAP_PREVIEW;
using rtype::client::testing::SECOND_VOTED_MAP;
using rtype::network::LobbyMap;
using rtype::network::LobbyPlayer;

void LobbyMapVoteFixture::open(std::vector<LobbyPlayer> players,
                               std::vector<LobbyMap> maps) {
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
      std::make_unique<RecordingInterfaceCanvas>(canvas_),
      std::make_unique<RecordingMapPreviews>());
  screen_->update(ONE_FRAME);
}

void LobbyMapVoteFixture::pressMapKey(bool held) {
  screen_->handleEvent(keyPress(MAP_KEY_CODE, AZERTY_M_PLACE));
  if (!held) {
    screen_->handleEvent(keyRelease(MAP_KEY_CODE, AZERTY_M_PLACE));
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

std::vector<std::string> LobbyMapVoteFixture::previewsDrawn() const {
  std::vector<std::string> drawn;
  for (const RecordedImage& image : canvas_.images) {
    if (image.source.size == MAP_PREVIEW_SIZE) {
      drawn.push_back(image.assetId);
    }
  }
  return drawn;
}

bool LobbyMapVoteFixture::hasFillOf(sf::Vector2i size, sf::Color color) const {
  return std::ranges::any_of(canvas_.fills, [&](const RecordedFill& fill) {
    return fill.area.size == size && fill.color == color;
  });
}

std::vector<LobbyMap> LobbyMapVoteFixture::twoMaps() {
  return {LobbyMap{.name = FIRST_VOTED_MAP, .preview = FIRST_MAP_PREVIEW},
          LobbyMap{.name = SECOND_VOTED_MAP, .preview = SECOND_MAP_PREVIEW}};
}
