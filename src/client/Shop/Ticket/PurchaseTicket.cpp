#include "PurchaseTicket.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "InterfacePixel.hpp"
#include "PlayfieldMapping.hpp"
#include "PurchaseTicketConstants.hpp"
#include "SoundConstants.hpp"
#include "TicketAnswer.hpp"
#include "TicketButton.hpp"
#include "TicketPainting.hpp"
#include "TicketView.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The answer pressing @p button gives. */
TicketAnswer answerOf(TicketButton button) {
  return button == TicketButton::Buy ? TicketAnswer::Buy : TicketAnswer::Cancel;
}

/** @returns The button at @p pixel of the interface, if any. */
std::optional<TicketButton> buttonAt(sf::Vector2i pixel) {
  const sf::Vector2i point = pixel - TICKET_AREA.position;
  if (TICKET_BUY_AREA.contains(point)) {
    return TicketButton::Buy;
  }
  if (TICKET_CANCEL_AREA.contains(point)) {
    return TicketButton::Cancel;
  }
  return std::nullopt;
}

/** @returns The other button than @p button. */
TicketButton otherThan(TicketButton button) {
  return button == TicketButton::Buy ? TicketButton::Cancel : TicketButton::Buy;
}

}  // namespace

PurchaseTicket::PurchaseTicket(ISoundPlayer& sounds) : sounds_(&sounds) {}

void PurchaseTicket::open(std::size_t item,
                          std::optional<sf::Keyboard::Key> heldKey) {
  view_ = TicketView{.item = item, .chosen = TicketButton::Buy, .paid = false};
  heldKey_ = heldKey;
  paidFor_ = {};
}

void PurchaseTicket::close() {
  view_.reset();
  heldKey_.reset();
}

void PurchaseTicket::paid() {
  if (view_.has_value()) {
    view_->paid = true;
    paidFor_ = {};
  }
}

bool PurchaseTicket::isOpen() const { return view_.has_value(); }

TicketAnswer PurchaseTicket::handleEvent(const sf::Event& event,
                                         const PlayfieldMapping& pointAt) {
  if (!view_.has_value() || view_->paid) {
    return TicketAnswer::None;
  }
  TicketView& view = *view_;
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    return keyPressed(view, pressed->code);
  }
  if (event.is<sf::Event::KeyReleased>() || event.is<sf::Event::FocusLost>()) {
    heldKey_.reset();
  } else if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    return pointed(view, interfacePixelAt(pointAt(moved->position)), false);
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    return pointed(view, interfacePixelAt(pointAt(click->position)), true);
  }
  return TicketAnswer::None;
}

void PurchaseTicket::advance(engine::Duration elapsed) {
  if (!view_.has_value() || !view_->paid) {
    return;
  }
  paidFor_ += elapsed;
  if (paidFor_ >= TICKET_PAID_TIME) {
    close();
  }
}

void PurchaseTicket::paint(IInterfaceCanvas& canvas) const {
  if (view_.has_value()) {
    paintTicket(canvas, *view_);
  }
}

TicketAnswer PurchaseTicket::keyPressed(TicketView& view,
                                        sf::Keyboard::Key key) {
  if (heldKey_ == key) {
    return TicketAnswer::None;
  }
  heldKey_ = key;
  if (key == MENU_CANCEL_KEY) {
    return TicketAnswer::Cancel;
  }
  if (key == MENU_CONFIRM_KEY) {
    return answerOf(view.chosen);
  }
  if (key == sf::Keyboard::Key::Left || key == sf::Keyboard::Key::Right ||
      key == sf::Keyboard::Key::Tab) {
    choose(view, otherThan(view.chosen));
  }
  return TicketAnswer::None;
}

TicketAnswer PurchaseTicket::pointed(TicketView& view, sf::Vector2i pixel,
                                     bool clicked) {
  const std::optional<TicketButton> button = buttonAt(pixel);
  if (button.has_value()) {
    choose(view, *button);
    return clicked ? answerOf(*button) : TicketAnswer::None;
  }
  if (clicked && !TICKET_AREA.contains(pixel)) {
    return TicketAnswer::Cancel;
  }
  return TicketAnswer::None;
}

void PurchaseTicket::choose(TicketView& view, TicketButton button) {
  if (view.chosen == button) {
    return;
  }
  view.chosen = button;
  sounds_->play(CLICK_SOUND_ID);
}

}  // namespace rtype::client
