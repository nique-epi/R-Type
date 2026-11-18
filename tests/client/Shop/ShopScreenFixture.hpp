#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include "FakeWallet.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ShopScreen.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A shop over a wallet the test sets, a recording navigator, a
 * recording audio service and a recording canvas; the player starts with the
 * starter kit and no kopeck. */
class ShopScreenFixture : public ::testing::Test {
 protected:
  ShopScreenFixture();

  /** @brief Presses then releases @p key, then runs a frame. */
  void press(sf::Keyboard::Key key);
  /** @brief Presses @p key without releasing it, then runs a frame. */
  void hold(sf::Keyboard::Key key);
  /** @brief Moves the pointer to @p pixel of the interface, at the pixel of
   * the window INTERFACE_PIXEL times as far, when @p clicked is false;
   * presses and releases the left button there otherwise; then runs a
   * frame. */
  void pointAt(sf::Vector2i pixel, bool clicked);
  /** @brief Presses Enter to show the ticket of the chosen ware, then Enter
   * again to press its BUY; the master then makes the purchase asked, if any,
   * its price leaving the kopecks; then runs a frame as long as its stamp
   * shows. */
  void buyChosen();
  /** @brief Runs a frame @p elapsed long. */
  void wait(engine::Duration elapsed);
  /** @brief Gives the player @p kopecks, then runs a frame. */
  void give(std::uint32_t kopecks);

  [[nodiscard]] FakeWallet& wallet();
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;
  [[nodiscard]] const InterfaceCanvasRecord& painting() const;

 private:
  FakeWallet wallet_;
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_;
  InterfaceCanvasRecord painting_;
  std::unique_ptr<ShopScreen> screen_;
};

/** @returns Whether @p painting shows the sales slip of a ware. */
[[nodiscard]] bool showsATicket(const InterfaceCanvasRecord& painting);

}  // namespace rtype::client::testing
