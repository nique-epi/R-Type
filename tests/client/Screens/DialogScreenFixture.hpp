#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "DialogScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @brief A dialog screen holding one button that counts its activations, with
 * a recording canvas. */
class DialogScreenFixture : public ::testing::Test {
 protected:
  DialogScreenFixture();

  rtype::client::DialogScreen& dialog();
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

  /** @returns How many times the button of the dialog ran its action. */
  [[nodiscard]] int activations() const;

  /** @brief Presses and releases a key. */
  void tap(sf::Keyboard::Key key);

 private:
  InterfaceCanvasRecord canvas_;
  int activations_ = 0;
  rtype::client::DialogScreen dialog_;
};
