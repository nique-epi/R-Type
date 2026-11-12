#include "ShopScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "Button.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfacePixel.hpp"
#include "KeyChipParts.hpp"
#include "LookItemPurchase.hpp"
#include "MenuBuilder.hpp"
#include "PlayerSettings.hpp"
#include "Purchase.hpp"
#include "Purse.hpp"
#include "SettingDefinitionConstants.hpp"
#include "ShelfSizes.hpp"
#include "ShopCatalogue.hpp"
#include "ShopPainting.hpp"
#include "ShopScreenConstants.hpp"
#include "ShopScreenParts.hpp"
#include "ShopShelf.hpp"
#include "ShopSpot.hpp"
#include "ShopStatus.hpp"
#include "ShopView.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

Button::Text backText() {
  return {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME};
}

/** @returns What @p settings lets the player buy with and already holds. */
Purse purseOf(const PlayerSettings& settings) {
  return Purse{.kopecks = settings.kopecks, .locker = settings.shipLocker};
}

}  // namespace

ShopScreen::ShopScreen(ShopScreenParts parts,
                       std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      settings_(&parts.settings.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      backChip_(KeyChipParts{.text = backText(),
                             .area = MenuBuilder::chipArea(backText()),
                             .key = MENU_CANCEL_KEY,
                             .action = [this] { navigator_->pop(); },
                             .sounds = parts.sounds}),
      wares_(waresForSale()),
      selection_(ShelfSizes{.looks = wares_.looks.size(),
                            .hulls = wares_.hulls.size()}) {
  repaint();
}

void ShopScreen::handleEvent(const sf::Event& event) {
  if (backChip_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(*pressed);
  } else if (event.is<sf::Event::KeyReleased>() ||
             event.is<sf::Event::FocusLost>()) {
    heldKey_.reset();
  } else if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    pointed(moved->position, false);
  } else if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>();
             wheel != nullptr && wheel->wheel == sf::Mouse::Wheel::Vertical) {
    selection_.scroll(wheel->delta > 0.0F ? -1 : 1);
    message_.reset();
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    pointed(click->position, true);
  }
}

void ShopScreen::update(engine::Duration /*elapsed*/) {
  static_cast<void>(settings_->saveIfChanged());
  repaint();
}

void ShopScreen::draw(sf::RenderTarget& target) const { canvas_->show(target); }

bool ShopScreen::hidesScreensBelow() const { return true; }

void ShopScreen::keyPressed(const sf::Event::KeyPressed& key) {
  if (selection_.move(key.code)) {
    sounds_->play(CLICK_SOUND_ID);
    message_.reset();
    return;
  }
  if (key.code == MENU_CONFIRM_KEY && heldKey_ != key.code) {
    heldKey_ = key.code;
    buy();
  }
}

void ShopScreen::pointed(sf::Vector2i position, bool clicked) {
  const ShopSpot before = selection_.spot();
  if (!selection_.chooseAt(interfacePixelAt(playfieldPointAt_(position)))) {
    return;
  }
  if (selection_.spot() != before) {
    message_.reset();
  }
  if (clicked) {
    buy();
  }
}

void ShopScreen::buy() {
  const std::size_t item = chosenItem();
  const Purse purse = purseOf(settings_->settings());
  const Purchase bought = buyLookItem(purse, item);
  if (bought.refusal.has_value()) {
    const std::uint32_t price = lookItemPrice(item);
    message_ = refusalLineOf(*bought.refusal,
                             price > purse.kopecks ? price - purse.kopecks : 0);
    return;
  }
  static_cast<void>(settings_->apply(KOPECKS_SETTING, bought.purse.kopecks));
  static_cast<void>(settings_->apply(SHIP_LOCKER_SETTING, bought.purse.locker));
  static_cast<void>(settings_->saveIfChanged());
  sounds_->play(PURCHASE_SOUND_ID);
  message_ = boughtLineOf(item);
}

void ShopScreen::repaint() {
  const Purse purse = purseOf(settings_->settings());
  const std::string status = message_.value_or(wareLineOf(chosenItem(), purse));
  canvas_->clear();
  paintShop(*canvas_, wares_,
            ShopView{.chosen = selection_.spot(),
                     .rows = selection_.window(),
                     .owned = purse.locker,
                     .kopecks = purse.kopecks,
                     .status = status});
  backChip_.paint(*canvas_);
  canvas_->display();
}

std::size_t ShopScreen::chosenItem() const {
  const ShopSpot spot = selection_.spot();
  return spot.shelf == ShopShelf::HullCase ? wares_.hulls.at(spot.place)
                                           : wares_.looks.at(spot.place);
}

}  // namespace rtype::client
