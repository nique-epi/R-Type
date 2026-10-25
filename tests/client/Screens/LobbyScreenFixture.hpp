#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include "LobbyScreen.hpp"
#include "LobbyState.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"

/** @brief A waiting room alone, the local player accepted as player 1, with
 * a recording navigator, connection and canvas, and READY bound to
 * READY_KEY. */
class LobbyScreenFixture : public ::testing::Test {
 protected:
  LobbyScreenFixture();

  /** @brief The server lists these players, and a frame goes by. */
  void list(std::vector<rtype::network::LobbyPlayer> players);

  /** @brief Presses then releases a key, by its place on the keyboard. */
  void tap(sf::Keyboard::Key key, sf::Keyboard::Scancode scancode);

  /** @returns The ready states asked of the connection, in order. */
  [[nodiscard]] std::vector<ServerConnectionCall> readyRequests() const;

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] RecordingServerConnection& connection();
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  [[nodiscard]] rtype::client::LobbyScreen& screen();

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  rtype::client::LobbyScreen screen_;
};
