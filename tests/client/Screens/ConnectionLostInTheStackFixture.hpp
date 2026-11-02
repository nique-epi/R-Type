#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <vector>
#include "IScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingScreen.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"

/**
 * @brief The connection lost dialog over the title in a real stack, as
 * ClientScreens opens it: what the dialog asks for is applied, so a test sees
 * the screens the player ends up with. Every other screen is a recording
 * screen.
 */
class ConnectionLostInTheStackFixture : public ::testing::Test {
 protected:
  ConnectionLostInTheStackFixture();

  /** @brief Presses and releases a key, lets the press last, applies what the
   * dialog asked for, then forgets what happened to the recording screens so
   * far. */
  void answerWith(sf::Keyboard::Key key);

  /** @brief Gives an event to the screen on top. */
  void sendAnEvent();

  /** @returns What happened to the recording screens since the answer. */
  [[nodiscard]] const std::vector<JournalEntry>& journal() const;

 private:
  std::unique_ptr<rtype::client::IScreen> build(rtype::client::ScreenId screen);

  InterfaceCanvasRecord dialogCanvas_;
  RecordingSoundPlayer sounds_;
  std::vector<JournalEntry> journal_;
  rtype::client::ScreenStack screens_;
};
