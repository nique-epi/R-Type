#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "CasinoEntranceScreen.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TimeConstants.hpp"

/** @brief The front of the casino alone, before its doors, its lights read
 * as none, with a recording navigator, canvas and sound player; the butler
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

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  std::unique_ptr<rtype::client::CasinoEntranceScreen> screen_;
};
