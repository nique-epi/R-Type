#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "ConnectionPhase.hpp"
#include "DialogScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A dialog of the client alone in its screen, with a recording
 * navigator and canvas. */
class DialogMenusFixture : public ::testing::Test {
 protected:
  /** @brief Opens the quit confirmation. */
  void openQuitConfirmation();

  /** @brief Opens the dialog of a session ended this way. */
  void openConnectionLost(rtype::client::ConnectionPhase ending);

  [[nodiscard]] const RecordingNavigator& navigator() const;

  /** @returns What the dialog painted. */
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

  /** @returns The sounds the dialog played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

  /** @brief Presses a key without releasing it. */
  void press(sf::Keyboard::Key key);

  /** @brief Presses and releases a key. */
  void tap(sf::Keyboard::Key key);

  /** @brief Lets a press last long enough to activate its button. */
  void letThePressLast();

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  std::optional<rtype::client::DialogScreen> dialog_;
};
