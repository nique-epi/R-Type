#include "ChatWindow.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include "BoundAction.hpp"
#include "ChatEntry.hpp"
#include "ChatPainting.hpp"
#include "ChatView.hpp"
#include "ChatWindowConstants.hpp"
#include "IChatChannel.hpp"
#include "IInterfaceCanvas.hpp"
#include "PlayerPalette.hpp"
#include "PlayerSettings.hpp"
#include "ProtocolConstants.hpp"
#include "TextField.hpp"

namespace rtype::client {

namespace {

/** @returns An empty field that takes a line of chat, at most
 * MAXIMUM_CHAT_TEXT_SIZE bytes of printable characters. */
TextField emptyDraft() {
  return TextField("", network::MAXIMUM_CHAT_TEXT_SIZE,
                   TextField::Characters::Printable);
}

/** @returns The key @p event presses when it falls to BoundAction::OpenChat,
 * as KeyBindings::boundActionFor() gives a key shared by two actions to the
 * first, while the chat is shown; nothing otherwise. */
std::optional<sf::Keyboard::Scancode> chatKeyPressedBy(
    const sf::Event& event, const PlayerSettings& settings) {
  const auto* pressed = event.getIf<sf::Event::KeyPressed>();
  if (pressed == nullptr || !settings.chatShown ||
      settings.keys.boundActionFor(pressed->scancode) !=
          BoundAction::OpenChat) {
    return std::nullopt;
  }
  return pressed->scancode;
}

/** @returns How far up the open chat may scroll: to its oldest line. */
int mostScrollOf(const IChatChannel& channel, const PlayerSettings& settings) {
  return std::max(0, chatLineCount(channel.entries(), settings.chatLetters) -
                         settings.chatOpenLines);
}

}  // namespace

ChatWindow::ChatWindow(IChatChannel& channel, const PlayerSettings& settings)
    : channel_(&channel), settings_(&settings), draft_(emptyDraft()) {}

bool ChatWindow::handleEvent(const sf::Event& event) {
  if (swallowsHeldKey(event)) {
    return true;
  }
  if (open_) {
    return followOpen(event);
  }
  if (const std::optional<sf::Keyboard::Scancode> key =
          chatKeyPressedBy(event, *settings_)) {
    open(*key);
    return true;
  }
  return false;
}

bool ChatWindow::isOpen() const { return open_; }

bool ChatWindow::isShown() const {
  if (open_) {
    return true;
  }
  return settings_->chatShown &&
         std::ranges::any_of(channel_->entries(),
                             [this](const ChatEntry& entry) {
                               return isShownClosed(entry, *settings_);
                             });
}

void ChatWindow::paint(IInterfaceCanvas& canvas, const PlayerPalette& palette,
                       int bottom) const {
  paintChat(
      canvas,
      ChatView{.entries = channel_->entries(),
               .settings = *settings_,
               .palette = palette,
               .draft = open_ ? std::optional<std::string_view>(draft_.text())
                              : std::nullopt,
               .scroll = scroll_,
               .bottom = bottom});
}

bool ChatWindow::swallowsHeldKey(const sf::Event& event) {
  if (event.is<sf::Event::FocusLost>()) {
    openingKey_.reset();
    closingKey_.reset();
    return false;
  }
  if (const auto* released = event.getIf<sf::Event::KeyReleased>()) {
    if (openingKey_ == released->scancode) {
      openingKey_.reset();
      return true;
    }
    if (closingKey_ == released->code) {
      closingKey_.reset();
      return true;
    }
    return false;
  }
  const auto* pressed = event.getIf<sf::Event::KeyPressed>();
  const bool repeat = pressed != nullptr && (openingKey_ == pressed->scancode ||
                                             closingKey_ == pressed->code);
  const bool typedByHeldKey =
      event.is<sf::Event::TextEntered>() &&
      (openingKey_.has_value() || closingKey_.has_value());
  return repeat || typedByHeldKey;
}

bool ChatWindow::followOpen(const sf::Event& event) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    if (pressed->code == CHAT_SEND_KEY) {
      send();
      close(CHAT_SEND_KEY);
    } else if (pressed->code == CHAT_CLOSE_KEY) {
      close(CHAT_CLOSE_KEY);
    } else if (pressed->code == OLDER_CHAT_KEY) {
      scroll_ = std::min(scroll_ + CHAT_SCROLLED_LINES,
                         mostScrollOf(*channel_, *settings_));
    } else if (pressed->code == NEWER_CHAT_KEY) {
      scroll_ = std::max(0, scroll_ - CHAT_SCROLLED_LINES);
    } else {
      draft_.handleEvent(event);
    }
    return true;
  }
  if (event.is<sf::Event::TextEntered>()) {
    draft_.handleEvent(event);
    return true;
  }
  return !event.is<sf::Event::FocusLost>() &&
         !event.is<sf::Event::FocusGained>();
}

void ChatWindow::open(sf::Keyboard::Scancode key) {
  open_ = true;
  openingKey_ = key;
  scroll_ = 0;
  draft_ = emptyDraft();
}

void ChatWindow::close(sf::Keyboard::Key key) {
  open_ = false;
  closingKey_ = key;
  draft_ = emptyDraft();
}

void ChatWindow::send() {
  const std::string& text = draft_.text();
  if (text.find_first_not_of(' ') != std::string::npos) {
    channel_->send(text);
  }
}

}  // namespace rtype::client
