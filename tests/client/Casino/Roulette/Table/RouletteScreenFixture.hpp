#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include "PlayerSettings.hpp"
#include "RecordingCasinoChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "RecordingWaitingRoom.hpp"
#include "RouletteMemory.hpp"
#include "RouletteRound.hpp"
#include "RouletteScreen.hpp"
#include "TimeConstants.hpp"

/** @brief A roulette table alone, ACE seated beside LUNA, its bets of A_ROUND
 * open, ACE holding CHIPS_OF_ACE chips, with a recording navigator, table,
 * canvas and sound player. */
class RouletteScreenFixture : public ::testing::Test {
 protected:
  RouletteScreenFixture();

  /** @brief Has the table send @p round, then lets a frame pass. */
  void showRound(const rtype::network::RouletteRound& round);
  /** @brief Gives the player @p chips in the settings, then lets a frame
   * pass. */
  void holdChips(std::uint32_t chips);
  /** @brief Presses and releases a key, then lets a frame pass. */
  void press(sf::Keyboard::Key key);
  /** @brief Presses a key without releasing it, then lets a frame pass. */
  void hold(sf::Keyboard::Key key);
  /** @brief Releases a key, then lets a frame pass. */
  void release(sf::Keyboard::Key key);
  /** @brief Lets @p elapsed pass in one frame. */
  void wait(rtype::engine::Duration elapsed);
  /** @brief Presses and releases the left button over @p interfacePixel, a
   * pixel of the interface picture, then lets a frame pass. */
  void clickAt(sf::Vector2i interfacePixel);

  [[nodiscard]] const RecordingCasinoChannel& casino() const;
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

 private:
  RecordingNavigator navigator_;
  RecordingCasinoChannel casino_;
  RecordingWaitingRoom waitingRoom_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::PlayerSettings settings_;
  rtype::client::RouletteMemory memory_;
  std::unique_ptr<rtype::client::RouletteScreen> screen_;
};
