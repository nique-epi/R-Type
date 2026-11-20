#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "CashierScreen.hpp"
#include "FakeWallet.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TimeConstants.hpp"

/** @brief A cashier just opened, buying chips, its cage read as empty, over
 * a wallet the test answers for the master, with a recording navigator,
 * canvas and sound player; the player holds KOPECKS_HELD kopecks and
 * CHIPS_HELD chips, and the cage moves unless a test opens it again with less
 * motion. */
class CashierScreenFixture : public ::testing::Test {
 protected:
  CashierScreenFixture();

  /** @brief Builds a new cashier in place of the one shown, moving as
   * @p motion says. */
  void reopenWith(rtype::client::MotionPreference motion);
  /** @brief Presses and releases a key, then lets a frame pass. */
  void press(sf::Keyboard::Key key);
  /** @brief Presses a key without releasing it, then lets a frame pass. */
  void hold(sf::Keyboard::Key key);
  /** @brief Releases a key, then lets a frame pass. */
  void release(sf::Keyboard::Key key);
  /** @brief Presses and releases the left button over @p interfacePixel, a
   * pixel of the interface picture, then lets a frame pass. */
  void clickAt(sf::Vector2i interfacePixel);
  /** @brief Lets a frame pass. */
  void wait();
  /** @brief Lets @p elapsed pass in one frame. */
  void waitFor(rtype::engine::Duration elapsed);

  [[nodiscard]] FakeWallet& wallet();
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

 private:
  FakeWallet wallet_;
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  InterfaceCanvasRecord canvas_;
  std::unique_ptr<rtype::client::CashierScreen> screen_;
};
