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
#include "Ambience.hpp"
#include "Button.hpp"
#include "IAudioService.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfacePixel.hpp"
#include "KeyChipParts.hpp"
#include "LookItemPurchase.hpp"
#include "MenuBuilder.hpp"
#include "MotionPreference.hpp"
#include "PlayerSettings.hpp"
#include "Purchase.hpp"
#include "Purse.hpp"
#include "SettingDefinitionConstants.hpp"
#include "ShelfSizes.hpp"
#include "ShopCatalogue.hpp"
#include "ShopPainting.hpp"
#include "ShopPaintingConstants.hpp"
#include "ShopScreenConstants.hpp"
#include "ShopScreenParts.hpp"
#include "ShopShelf.hpp"
#include "ShopSpot.hpp"
#include "ShopStatus.hpp"
#include "ShopView.hpp"
#include "ShopkeeperMotion.hpp"
#include "ShopkeeperMotionConstants.hpp"
#include "SoundConstants.hpp"
#include "TextMetrics.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns What BACK says, at the text size of @p metrics. */
Button::Text backText(TextMetrics metrics) {
  return {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME, .size = metrics};
}

/** @returns What @p settings lets the player buy with and already holds. */
Purse purseOf(const PlayerSettings& settings) {
  return Purse{.kopecks = settings.kopecks, .locker = settings.shipLocker};
}

/** @brief Changes what the shop plays: the music of the radio instead of
 * the music of the shop, or back. */
void tuneRadio(IAudioService& audio) {
  audio.setAmbience(audio.ambience() == Ambience::Shop ? Ambience::ShopRadio
                                                       : Ambience::Shop);
}

}  // namespace

ShopScreen::ShopScreen(ShopScreenParts parts,
                       std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      audio_(&parts.audio.get()),
      music_(parts.audio.get(), Ambience::Shop),
      settings_(&parts.settings.get()),
      motion_(parts.motion),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      backChip_(KeyChipParts{
          .text = backText(canvas_->textMetrics()),
          .area = MenuBuilder::chipArea(backText(canvas_->textMetrics())),
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

void ShopScreen::update(engine::Duration elapsed) {
  elapsed_ += elapsed;
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
  if (heldKey_ == key.code) {
    return;
  }
  if (key.code == MENU_CONFIRM_KEY) {
    heldKey_ = key.code;
    buy();
  } else if (key.code == SHOP_RADIO_KEY) {
    heldKey_ = key.code;
    tuneRadio(*audio_);
  }
}

void ShopScreen::pointed(sf::Vector2i position, bool clicked) {
  const sf::Vector2i pixel = interfacePixelAt(playfieldPointAt_(position));
  radioPointed_ = SHOP_RADIO_AREA.contains(pixel);
  if (radioPointed_) {
    if (clicked) {
      tuneRadio(*audio_);
    }
    return;
  }
  const ShopSpot before = selection_.spot();
  if (!selection_.chooseAt(pixel)) {
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
  sounds_->play(PURCHASE_SOUND_ID);
  message_ = boughtLineOf(item);
}

void ShopScreen::repaint() {
  const Purse purse = purseOf(settings_->settings());
  const std::string status =
      radioPointed_ ? std::string(SHOP_RADIO_HINT)
                    : message_.value_or(wareLineOf(chosenItem(), purse));
  canvas_->clear();
  paintShop(*canvas_, wares_,
            ShopView{.chosen = selection_.spot(),
                     .rows = selection_.window(),
                     .owned = purse.locker,
                     .kopecks = purse.kopecks,
                     .status = status,
                     .keeper = motion_ == MotionPreference::Full
                                   ? shopkeeperPoseAt(elapsed_)
                                   : SHOPKEEPER_AT_REST,
                     .radioPointed = radioPointed_});
  backChip_.paint(*canvas_);
  canvas_->display();
}

std::size_t ShopScreen::chosenItem() const {
  const ShopSpot spot = selection_.spot();
  return spot.shelf == ShopShelf::HullCase ? wares_.hulls.at(spot.place)
                                           : wares_.looks.at(spot.place);
}

}  // namespace rtype::client
