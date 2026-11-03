#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "PlayerSettings.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TitleScreen.hpp"

/** @brief A title screen alone, with a recording navigator and canvas. */
class TitleScreenFixture : public ::testing::Test {
 protected:
  TitleScreenFixture();

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  rtype::client::TitleScreen& title();

  /** @brief Presses and releases a key, then lets the press last. */
  void choose(sf::Keyboard::Key key);

  /** @returns The sounds the title played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::PlayerSettings settings_;
  rtype::client::TitleScreen title_;
};
