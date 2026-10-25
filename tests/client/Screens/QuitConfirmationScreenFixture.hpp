#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "QuitConfirmationScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

/** @brief A quit confirmation alone, with a recording navigator and
 * canvas. */
class QuitConfirmationScreenFixture : public ::testing::Test {
 protected:
  QuitConfirmationScreenFixture();

  [[nodiscard]] const RecordingNavigator& navigator() const;
  rtype::client::QuitConfirmationScreen& dialog();

  /** @brief Presses and releases a key. */
  void tap(sf::Keyboard::Key key);

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  rtype::client::QuitConfirmationScreen dialog_;
};
