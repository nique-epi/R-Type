#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "FakeAccountSession.hpp"
#include "FakeWallet.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingCasinoChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "RouletteMemory.hpp"
#include "RouletteScreen.hpp"
#include "TimeConstants.hpp"

/** @brief A roulette table alone, ACE signed in and seated beside LUNA in
 * NEBULA_LOUNGE, the bets of A_ROUND open, ACE holding CHIPS_OF_ACE chips,
 * with a recording navigator, casino, canvas and sound player. A test changes
 * the casino or the wallet, then lets a frame pass. */
class RouletteScreenFixture : public ::testing::Test {
 protected:
  RouletteScreenFixture();

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

  [[nodiscard]] RecordingCasinoChannel& casino();
  [[nodiscard]] FakeWallet& wallet();
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

 private:
  RecordingNavigator navigator_;
  RecordingCasinoChannel casino_;
  FakeAccountSession account_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_{sounds_};
  FakeWallet wallet_;
  rtype::client::RouletteMemory memory_;
  std::unique_ptr<rtype::client::RouletteScreen> screen_;
};
