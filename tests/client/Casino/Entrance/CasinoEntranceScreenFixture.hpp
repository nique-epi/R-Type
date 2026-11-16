#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "CasinoEntranceScreen.hpp"
#include "FakeWallet.hpp"
#include "MotionPreference.hpp"
#include "RecordingCasinoChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TimeConstants.hpp"

/** @brief A casino alone, before its doors, with a recording navigator,
 * casino, canvas and sound player, the player holding KOPECKS_HELD kopecks and
 * CHIPS_HELD chips, the master listing a full room then FREE_ROOM; the butler
 * moves unless a test opens it again with less motion. */
class CasinoEntranceScreenFixture : public ::testing::Test {
 protected:
  CasinoEntranceScreenFixture();

  /** @brief Builds a new casino in place of the one shown, the butler moving
   * as @p motion says. */
  void reopenWith(rtype::client::MotionPreference motion);
  /** @brief Presses and releases a key, then lets a frame pass. */
  void press(sf::Keyboard::Key key);
  /** @brief Presses a key without releasing it, then lets a frame pass. */
  void hold(sf::Keyboard::Key key);
  /** @brief Releases a key, then lets a frame pass. */
  void release(sf::Keyboard::Key key);
  /** @brief Lets @p elapsed pass in one frame. */
  void wait(rtype::engine::Duration elapsed);
  /** @brief Enters, then lets the doors open: the hall shows. */
  void enterTheHall();
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
  FakeWallet wallet_;
  std::unique_ptr<rtype::client::CasinoEntranceScreen> screen_;
};
