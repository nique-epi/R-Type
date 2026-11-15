#include "CashierScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <utility>
#include "CashierAmount.hpp"
#include "CashierConstants.hpp"
#include "CashierLayout.hpp"
#include "CashierPainting.hpp"
#include "CashierRules.hpp"
#include "CashierScreenParts.hpp"
#include "CashierView.hpp"
#include "CashierWay.hpp"
#include "CasinoBarChip.hpp"
#include "Exchange.hpp"
#include "InterfacePixel.hpp"
#include "PlayerPurse.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SoundConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The place of the step @p key chooses, if it chooses one. */
std::optional<std::size_t> stepOfKey(sf::Keyboard::Key key) {
  for (std::size_t place = 0; place < CASHIER_STEP_KEYS.size(); ++place) {
    if (CASHIER_STEP_KEYS.at(place) == key) {
      return place;
    }
  }
  return std::nullopt;
}

}  // namespace

CashierScreen::CashierScreen(CashierScreenParts parts,
                             std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      settings_(&parts.settings.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      way_(parts.way),
      confirmChip_(chipOf(
          parts.way == CashierWay::BuyChips ? BUY_CHIPS_CONFIRM_BAR
                                            : CASH_OUT_CONFIRM_BAR,
          [this] { confirm(); }, parts.sounds)),
      backChip_(chipOf(
          CASHIER_BACK_BAR, [this] { navigator_->pop(); }, parts.sounds)) {
  repaint();
}

void CashierScreen::handleEvent(const sf::Event& event) {
  if (confirmChip_.handleEvent(event, playfieldPointAt_) ||
      backChip_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(*pressed);
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    pointerPressed(interfacePixelAt(playfieldPointAt_(click->position)));
  }
}

void CashierScreen::update([[maybe_unused]] engine::Duration elapsed) {
  repaint();
}

void CashierScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool CashierScreen::hidesScreensBelow() const { return false; }

void CashierScreen::keyPressed(const sf::Event::KeyPressed& key) {
  if (key.code == CASHIER_LOWER_KEY) {
    amount_.lower();
  } else if (key.code == CASHIER_RAISE_KEY) {
    amount_.raise();
  } else if (const std::optional<std::size_t> step = stepOfKey(key.code)) {
    amount_.chooseStep(*step);
  } else {
    return;
  }
  sounds_->play(CLICK_SOUND_ID);
}

void CashierScreen::pointerPressed(sf::Vector2i pixel) {
  if (CASHIER_LOWER_MARK_AREA.contains(pixel)) {
    amount_.lower();
  } else if (CASHIER_RAISE_MARK_AREA.contains(pixel)) {
    amount_.raise();
  } else if (const std::optional<std::size_t> chip = selectorChipAt(pixel)) {
    amount_.chooseStep(*chip);
  } else {
    return;
  }
  sounds_->play(CLICK_SOUND_ID);
}

void CashierScreen::confirm() {
  const Exchange made = tried();
  if (made.refusal.has_value()) {
    return;
  }
  static_cast<void>(settings_->apply(KOPECKS_SETTING, made.purse.kopecks));
  static_cast<void>(settings_->apply(CHIPS_SETTING, made.purse.chips));
  static_cast<void>(settings_->saveIfChanged());
  navigator_->pop();
}

Exchange CashierScreen::tried() const {
  return exchangeAt(purseOf(settings_->settings()), way_, amount_.amount());
}

void CashierScreen::repaint() {
  const Exchange exchange = tried();
  confirmChip_.setEnabled(!exchange.refusal.has_value());
  canvas_->clear();
  paintCashier(*canvas_, CashierView{.way = way_,
                                     .purse = purseOf(settings_->settings()),
                                     .amount = amount_.amount(),
                                     .step = amount_.step(),
                                     .tried = exchange});
  confirmChip_.paint(*canvas_);
  backChip_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
