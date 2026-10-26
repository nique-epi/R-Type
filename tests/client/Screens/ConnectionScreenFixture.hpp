#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <string_view>
#include "ConnectionScreen.hpp"
#include "ConnectionStatus.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A connection screen alone, with a recording navigator, connection
 * and canvas, its fields started with the launch values. */
class ConnectionScreenFixture : public ::testing::Test {
 protected:
  ConnectionScreenFixture();

  [[nodiscard]] const RecordingNavigator& navigator() const;
  RecordingServerConnection& connection();
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  rtype::client::ConnectionScreen& screen();

  /** @brief Presses and releases a key. */
  void tap(sf::Keyboard::Key key);

  /** @brief Presses and releases a key, then lets the press last. */
  void choose(sf::Keyboard::Key key);

  /** @brief Types a text, one character after the other. */
  void type(std::u32string_view text);

  /** @brief Erases characters from the selected field, Backspace after
   * Backspace. */
  void erase(std::size_t characters);

  /** @brief Gives the connection a status, then lets one frame go by. */
  void show(rtype::client::ConnectionStatus status);

  /** @returns The sounds the screen played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::ConnectionScreen screen_;
};
