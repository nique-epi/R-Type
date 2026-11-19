#pragma once

#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "LobbyMap.hpp"
#include "LobbyScreen.hpp"
#include "LobbyState.hpp"
#include "PlayerSettings.hpp"
#include "RecordingChatChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingMapBallot.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingShipLookExchange.hpp"
#include "RecordingSoundPlayer.hpp"
#include "RecordingWaitingRoom.hpp"
#include "ShipLook.hpp"

/** @brief A waiting room listing maps to vote for, the local player accepted
 * as player 1, over a recording canvas. LobbyScreenFixture holds more member
 * functions than a class may, so these tests have a fixture of their own. */
class LobbyMapVoteFixture : public ::testing::Test {
 protected:
  /** @brief Opens the room listing @p players and @p maps, then lets a frame
   * go by. */
  void open(std::vector<rtype::network::LobbyPlayer> players,
            std::vector<rtype::network::LobbyMap> maps);

  /** @brief Presses MAP_KEY as an AZERTY keyboard sends it, the letter M at
   * the place of a QWERTY semicolon, then releases it unless @p held. */
  void pressMapKey(bool held);

  /** @brief Clicks the window at @p pixel, then lets a press go by. */
  void click(sf::Vector2i pixel);

  /** @returns The two maps of the tests, FIRST_VOTED_MAP then
   * SECOND_VOTED_MAP, with their previews. */
  [[nodiscard]] static std::vector<rtype::network::LobbyMap> twoMaps();

  /** @returns The votes the room asked for, in order. */
  [[nodiscard]] const std::vector<std::uint8_t>& votes() const;

  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

  /** @returns The asset ids of the images painted the size of a preview of
   * the card, in order. */
  [[nodiscard]] std::vector<std::string> previewsDrawn() const;

  /** @returns Whether a fill of @p size in @p color was painted. */
  [[nodiscard]] bool hasFillOf(sf::Vector2i size, sf::Color color) const;

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  RecordingWaitingRoom waitingRoom_;
  RecordingShipLookExchange shipLooks_;
  RecordingMapBallot mapBallot_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::PlayerSettings settings_;
  RecordingChatChannel chat_;
  rtype::identifiers::ShipLook look_{};
  std::unique_ptr<rtype::client::LobbyScreen> screen_;
};
