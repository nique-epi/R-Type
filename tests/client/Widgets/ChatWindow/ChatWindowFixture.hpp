#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <string_view>
#include "ChatWindow.hpp"
#include "PlayerSettings.hpp"
#include "RecordingChatChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @brief A chat over a recording channel, with the settings of a first
 * launch, painted on a recording canvas. */
class ChatWindowFixture : public ::testing::Test {
 protected:
  /** @brief Presses then releases a key.
   * @returns Whether the chat took both. */
  bool tap(sf::Keyboard::Key key, sf::Keyboard::Scancode scancode);

  /** @brief Types each letter of @p text, as TextEntered events. */
  void type(std::string_view text);

  /** @brief Opens the chat with its key, as the keyboard does: the press, the
   * letter it types, the release. */
  void openChat();

  /** @brief Receives @p count lines of NOVA, "line 1" to "line <count>". */
  void receiveNumberedLines(int count);

  /** @returns What the chat paints on a cleared canvas. */
  [[nodiscard]] const InterfaceCanvasRecord& painted();

  [[nodiscard]] rtype::client::ChatWindow& chat();
  [[nodiscard]] RecordingChatChannel& channel();
  [[nodiscard]] rtype::client::PlayerSettings& settings();

 private:
  RecordingChatChannel channel_;
  rtype::client::PlayerSettings settings_;
  InterfaceCanvasRecord record_;
  RecordingInterfaceCanvas canvas_{record_};
  rtype::client::ChatWindow chat_{channel_, settings_};
};
