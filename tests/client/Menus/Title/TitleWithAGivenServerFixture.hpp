#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "PlayerSettings.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TitleScreen.hpp"

/** @brief A title screen of a client launched with `-h`, alone, with a
 * recording navigator. */
class TitleWithAGivenServerFixture : public ::testing::Test {
 protected:
  TitleWithAGivenServerFixture();

  [[nodiscard]] const RecordingNavigator& navigator() const;

  /** @brief Presses and releases a key, then lets the press last. */
  void choose(sf::Keyboard::Key key);

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::PlayerSettings settings_;
  rtype::client::TitleScreen title_;
};
