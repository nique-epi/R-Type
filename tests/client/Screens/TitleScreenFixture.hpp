#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
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

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  rtype::client::TitleScreen title_;
};
