#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "IChatChannel.hpp"
#include "IInterfaceCanvas.hpp"
#include "PlayerPalette.hpp"
#include "PlayerSettings.hpp"
#include "TextField.hpp"

namespace rtype::client {

/**
 * @brief The chat of the waiting room and of the game. Closed, it shows the
 * newest lines of the session over the screen; the key bound to
 * BoundAction::OpenChat opens it, and from then on it takes every key until
 * ENTER sends the line typed or ESC closes it: no key reaches the screen
 * meanwhile. The press that opens it, its repeats, the letter it types and its
 * release never reach the field nor the screen, nor do the repeats and the
 * release of the key that closes it: the game opens its options on the
 * release of ESC.
 */
class ChatWindow {
 public:
  /** @param channel The chat of the session, which must outlive this object.
   * @param settings The settings of the player, read at each event and each
   * paint; they must outlive this object. */
  ChatWindow(IChatChannel& channel, const PlayerSettings& settings);

  /** @returns Whether the chat took @p event, which the screen must then
   * ignore. */
  bool handleEvent(const sf::Event& event);

  [[nodiscard]] bool isOpen() const;

  /** @returns Whether the chat paints anything: open, or a line still
   * shows. */
  [[nodiscard]] bool isShown() const;

  /** @brief Paints the chat, its lines ending on the row @p bottom. */
  void paint(IInterfaceCanvas& canvas, const PlayerPalette& palette,
             int bottom) const;

 private:
  /** @returns Whether @p event repeats, types with or releases the key that
   * opened or closed the chat; it forgets that key on its release. */
  bool swallowsHeldKey(const sf::Event& event);
  bool followOpen(const sf::Event& event);
  void open(sf::Keyboard::Scancode key);
  void close(sf::Keyboard::Key key);
  void send();

  IChatChannel* channel_;
  const PlayerSettings* settings_;
  TextField draft_;
  std::optional<sf::Keyboard::Scancode> openingKey_;
  std::optional<sf::Keyboard::Key> closingKey_;
  int scroll_ = 0;
  bool open_ = false;
};

}  // namespace rtype::client
