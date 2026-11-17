#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "PlayfieldMapping.hpp"
#include "TicketAnswer.hpp"
#include "TicketButton.hpp"
#include "TicketView.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The sales slip the shop shows over the booth before it sells a
 * ware, so a stray click or Enter buys nothing: the ware, what it is, its
 * price, then CANCEL and BUY.
 *
 * BUY is chosen when it opens. Left, Right or Tab move the choice, with a
 * click; the pointer chooses the button it moves over. Enter presses the
 * chosen button, once per hold: the key that opened the ticket, still held,
 * does nothing until it is released. A click on a button presses it, Escape
 * or a click off the ticket cancels it. Once the purchase is made, paid()
 * shows its stamp for TICKET_PAID_TIME, every event ignored, then it
 * closes.
 */
class PurchaseTicket {
 public:
  explicit PurchaseTicket(ISoundPlayer& sounds);

  /** @brief Shows the ticket of @p item, a position in SHIP_LOOK_ITEM_NAMES,
   * BUY chosen; @p heldKey is the key held when it opens, if any. */
  void open(std::size_t item, std::optional<sf::Keyboard::Key> heldKey);
  /** @brief Hides the ticket. */
  void close();
  /** @brief Stamps the ticket PAID, then closes it after TICKET_PAID_TIME. */
  void paid();
  /** @returns Whether the ticket shows. */
  [[nodiscard]] bool isOpen() const;
  /** @returns What @p event answered, @p pointAt placing the pointer. */
  TicketAnswer handleEvent(const sf::Event& event,
                           const PlayfieldMapping& pointAt);
  /** @brief Closes a paid ticket once its stamp showed long enough. */
  void advance(engine::Duration elapsed);
  /** @brief Paints the ticket over the shop; nothing while it is closed. */
  void paint(IInterfaceCanvas& canvas) const;

 private:
  /** @returns What the press of @p key answered on the ticket @p view. */
  TicketAnswer keyPressed(TicketView& view, sf::Keyboard::Key key);
  /** @returns What the pointer at @p pixel of the interface answered on the
   * ticket @p view, pressed when @p clicked. */
  TicketAnswer pointed(TicketView& view, sf::Vector2i pixel, bool clicked);
  /** @brief Chooses @p button on the ticket @p view, with a click when the
   * choice moves. */
  void choose(TicketView& view, TicketButton button);

  ISoundPlayer* sounds_;
  std::optional<TicketView> view_;
  std::optional<sf::Keyboard::Key> heldKey_;
  /** @brief How long the paid ticket has shown its stamp. */
  engine::Duration paidFor_{};
};

}  // namespace rtype::client
