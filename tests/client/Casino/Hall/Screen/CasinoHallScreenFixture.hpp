#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "CasinoHallScreen.hpp"
#include "MotionPreference.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingCasinoChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TimeConstants.hpp"

/** @brief The hall of the casino alone, its picture read as empty, with a
 * recording navigator, casino, canvas and sound player, the master listing a
 * full room then HALL_FREE_ROOM; it moves unless a test opens it again with
 * less motion. */
class CasinoHallScreenFixture : public ::testing::Test {
 protected:
  CasinoHallScreenFixture();

  /** @brief Builds a new hall in place of the one shown, moving as
   * @p motion says. */
  void reopenWith(rtype::client::MotionPreference motion);
  /** @brief Presses and releases a key, then lets a frame pass. */
  void press(sf::Keyboard::Key key);
  /** @brief Presses a key without releasing it, then lets a frame pass. */
  void hold(sf::Keyboard::Key key);
  /** @brief Releases a key, then lets a frame pass. */
  void release(sf::Keyboard::Key key);
  /** @brief Presses and releases the left button at @p windowPixel, then
   * lets a frame pass. */
  void click(sf::Vector2i windowPixel);
  /** @brief Lets @p elapsed pass in one frame. */
  void wait(rtype::engine::Duration elapsed);
  /** @brief Has the master list the full room alone, then lets a frame
   * pass. */
  void fillEveryRoom();

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const RecordingCasinoChannel& casino() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

 private:
  RecordingNavigator navigator_;
  RecordingCasinoChannel casino_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_{sounds_};
  std::unique_ptr<rtype::client::CasinoHallScreen> screen_;
};
