#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include "LobbyPlace.hpp"
#include "LobbyScreen.hpp"
#include "LobbyState.hpp"
#include "PlayerSettings.hpp"
#include "RecordingChatChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingShipLookExchange.hpp"
#include "RecordingSoundPlayer.hpp"
#include "RecordingWaitingRoom.hpp"
#include "ShipLook.hpp"
#include "TextMetrics.hpp"

/** @brief A waiting room alone, the local player accepted as player 1, with
 * a recording navigator, connection and canvas, and the bindings of a first
 * launch, which bind READY to READY_KEY. */
class LobbyScreenFixture : public ::testing::Test {
 protected:
  explicit LobbyScreenFixture(rtype::client::LobbyPlace place =
                                  rtype::client::LobbyPlace::AfterConnection,
                              rtype::client::TextMetrics metrics = {});

  /** @brief The server lists these players, and a frame goes by. */
  void list(std::vector<rtype::network::LobbyPlayer> players);

  /** @brief Presses then releases a key, by its place on the keyboard. */
  void tap(sf::Keyboard::Key key, sf::Keyboard::Scancode scancode);

  /** @returns The ready states asked of the waiting room, in order. */
  [[nodiscard]] const std::vector<bool>& readyRequests() const;

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] RecordingServerConnection& connection();
  [[nodiscard]] RecordingWaitingRoom& waitingRoom();
  [[nodiscard]] RecordingShipLookExchange& shipLooks();
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  [[nodiscard]] rtype::client::LobbyScreen& screen();
  /** @returns What the ship of the local player wears, as the settings
   * would hold it; the room reads it at each frame. */
  [[nodiscard]] rtype::identifiers::ShipLook& look();

  /** @returns The sounds the waiting room played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

  /** @returns The chat of the session, and what the room sent to it. */
  [[nodiscard]] const RecordingChatChannel& chat() const;

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  RecordingWaitingRoom waitingRoom_;
  RecordingShipLookExchange shipLooks_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::PlayerSettings settings_;
  RecordingChatChannel chat_;
  rtype::identifiers::ShipLook look_{};
  rtype::client::LobbyScreen screen_;
};
