#include "ChatWindowFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <string_view>
#include "ChatEntry.hpp"
#include "ChatWindow.hpp"
#include "ChatWindowTestConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "KeyPress.hpp"
#include "PlayerSettings.hpp"
#include "RecordingChatChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

using rtype::client::ChatEntry;
using rtype::client::ChatWindow;
using rtype::client::PLAYER_COLORS;
using rtype::client::PlayerSettings;
using rtype::client::testing::CHAT_KEY;
using rtype::client::testing::CHAT_KEY_LETTER;
using rtype::client::testing::CHAT_TEST_BOTTOM;
using rtype::client::testing::LINE_PREFIX;
using rtype::client::testing::NOVA;
using rtype::client::testing::NOVA_NUMBER;

bool ChatWindowFixture::tap(sf::Keyboard::Key key,
                            sf::Keyboard::Scancode scancode) {
  const bool pressTaken = chat_.handleEvent(keyPress(key, scancode));
  const bool releaseTaken = chat_.handleEvent(keyRelease(key, scancode));
  return pressTaken && releaseTaken;
}

void ChatWindowFixture::type(std::string_view text) {
  for (const char letter : text) {
    chat_.handleEvent(typedCharacter(static_cast<char32_t>(letter)));
  }
}

void ChatWindowFixture::openChat() {
  chat_.handleEvent(keyPress(sf::Keyboard::Key::T, CHAT_KEY));
  chat_.handleEvent(typedCharacter(CHAT_KEY_LETTER));
  chat_.handleEvent(keyRelease(sf::Keyboard::Key::T, CHAT_KEY));
}

void ChatWindowFixture::receiveNovaLine(std::string_view text,
                                        rtype::engine::Duration age) {
  channel_.receive(ChatEntry{.playerNumber = NOVA_NUMBER,
                             .nickname = std::string{NOVA},
                             .text = std::string{text},
                             .age = age});
}

void ChatWindowFixture::receiveNumberedLines(int count) {
  for (int line = 1; line <= count; ++line) {
    receiveNovaLine(std::string{LINE_PREFIX} + std::to_string(line), {});
  }
}

const InterfaceCanvasRecord& ChatWindowFixture::painted() {
  canvas_.clear();
  chat_.paint(canvas_, PLAYER_COLORS, CHAT_TEST_BOTTOM);
  return record_;
}

ChatWindow& ChatWindowFixture::chat() { return chat_; }

RecordingChatChannel& ChatWindowFixture::channel() { return channel_; }

PlayerSettings& ChatWindowFixture::settings() { return settings_; }
