#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include "PlayerSettings.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SettingsService.hpp"
#include "ShopScreen.hpp"

namespace rtype::client::testing {

/** @brief A shop over settings saved to a recording storage, a recording
 * navigator and a recording canvas; the player starts with the starter kit
 * and no kopeck. */
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
  /** @brief Gives the player @p kopecks, then runs a frame. */
  void give(std::uint32_t kopecks);

  [[nodiscard]] const PlayerSettings& settings() const;
  [[nodiscard]] const RecordingSettingsStorage& storage() const;
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;
  [[nodiscard]] const InterfaceCanvasRecord& painting() const;

 private:
  PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  SettingsService service_;
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  InterfaceCanvasRecord painting_;
  std::unique_ptr<ShopScreen> screen_;
};

}  // namespace rtype::client::testing
