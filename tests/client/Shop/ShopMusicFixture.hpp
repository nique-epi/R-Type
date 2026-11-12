#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "Ambience.hpp"
#include "PlayerSettings.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SettingsService.hpp"
#include "ShopScreen.hpp"

namespace rtype::client::testing {

/** @brief A shop opened over a screen that holds the space ambience, as the
 * server map does, on an audio service that records the ambience asked. */
class ShopMusicFixture : public ::testing::Test {
 protected:
  ShopMusicFixture();

  /** @brief Presses then releases @p key. */
  void press(sf::Keyboard::Key key);
  /** @brief Presses @p key without releasing it. */
  void hold(sf::Keyboard::Key key);
  /** @brief Presses the left button at @p pixel of the interface, at the
   * pixel of the window INTERFACE_PIXEL times as far. */
  void click(sf::Vector2i pixel);
  /** @brief Closes the shop, as the screen stack does when it is popped. */
  void close();

  /** @returns The ambience the audio service plays now. */
  [[nodiscard]] Ambience ambience() const;

 private:
  PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  SettingsService service_;
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_;
  InterfaceCanvasRecord painting_;
  std::unique_ptr<ShopScreen> screen_;
};

}  // namespace rtype::client::testing
