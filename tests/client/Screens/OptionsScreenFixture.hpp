#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "FakeSizableWindow.hpp"
#include "OptionsPlace.hpp"
#include "OptionsScreen.hpp"
#include "PlayerSettings.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief The options alone, over the title unless told otherwise, on a
 * DESKTOP whose window is 1600 x 900, with a recording navigator, storage and
 * canvas. */
class OptionsScreenFixture : public ::testing::Test {
 protected:
  explicit OptionsScreenFixture(rtype::client::OptionsPlace place =
                                    rtype::client::OptionsPlace::OverTitle);

  /** @brief Presses and releases a key, then lets a frame pass that is as
   * long as a press. */
  void tap(sf::Keyboard::Key key,
           sf::Keyboard::Scancode scancode = sf::Keyboard::Scan::Unknown);

  /** @brief Taps the key at the place of E, which opens the next tab. */
  void openNextTab();

  rtype::client::PlayerSettings& settings();
  RecordingSettingsStorage& storage();
  FakeSizableWindow& window();
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  rtype::client::OptionsScreen& screen();

  /** @returns The sounds the options played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  rtype::client::PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  FakeSizableWindow window_;
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::OptionsScreen screen_;
};
