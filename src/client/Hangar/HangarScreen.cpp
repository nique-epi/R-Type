#include "HangarScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "Button.hpp"
#include "DockPainting.hpp"
#include "DockScene.hpp"
#include "FittingRun.hpp"
#include "FittingTimeline.hpp"
#include "HangarScreenConstants.hpp"
#include "HangarScreenIdConstants.hpp"
#include "HangarScreenParts.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfacePixel.hpp"
#include "KeyChipParts.hpp"
#include "LockerChoice.hpp"
#include "LockerPainting.hpp"
#include "LockerView.hpp"
#include "MenuBuilder.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingDefinitionConstants.hpp"
#include "ShipLocker.hpp"
#include "ShipLook.hpp"
#include "ShipLookSpriteConstants.hpp"
#include "ShipLooks.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

using identifiers::ShipLook;

namespace {

/** @returns The item whose cell lies under @p position of the window. */
std::optional<std::size_t> itemUnder(sf::Vector2i position,
                                     const PlayfieldMapping& playfieldPointAt) {
  return lockerItemAt(interfacePixelAt(playfieldPointAt(position)));
}

Button::Text backText() {
  return {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME};
}

/** @returns The status line: the name of @p item, then what Enter does with
 * it, or that an arm is fitting an item. */
std::string statusOf(std::size_t item, ShipLook worn, bool fitting,
                     const ShipLocker& locker) {
  if (!locker.items.test(item)) {
    return LOCKED_STATUS;
  }
  const std::string status(SHIP_LOOK_SPRITES.at(item).label);
  if (fitting) {
    return status + FITTING_HINT;
  }
  return status + (wears(worn, item) ? TAKE_OFF_HINT : WEAR_HINT);
}

}  // namespace

HangarScreen::HangarScreen(HangarScreenParts parts,
                           std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      settings_(&parts.settings.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      palette_(parts.palette),
      shipColor_(parts.shipColor),
      motion_(parts.motion),
      backChip_(KeyChipParts{.text = backText(),
                             .area = MenuBuilder::chipArea(backText()),
                             .key = MENU_CANCEL_KEY,
                             .action = [this] { navigator_->pop(); },
                             .sounds = parts.sounds}) {
  repaint();
}

void HangarScreen::handleEvent(const sf::Event& event) {
  if (backChip_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(*pressed);
  } else if (event.is<sf::Event::KeyReleased>() ||
             event.is<sf::Event::FocusLost>()) {
    confirmHeld_ = false;
  } else if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    if (const std::optional<std::size_t> item =
            itemUnder(moved->position, playfieldPointAt_)) {
      chosen_ = *item;
    }
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    if (const std::optional<std::size_t> item =
            itemUnder(click->position, playfieldPointAt_)) {
      chosen_ = *item;
      choose();
    }
  }
}

void HangarScreen::update(engine::Duration elapsed) {
  elapsed_ += elapsed;
  if (fitting_.has_value() && fittingProgressOf(*fitting_, elapsed_) >= 1.0F) {
    fitting_.reset();
  }
  static_cast<void>(settings_->saveIfChanged());
  repaint();
}

void HangarScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool HangarScreen::hidesScreensBelow() const { return true; }

void HangarScreen::keyPressed(const sf::Event::KeyPressed& key) {
  if (const std::optional<std::size_t> next =
          lockerItemTowards(chosen_, key.code)) {
    if (*next != chosen_) {
      sounds_->play(CLICK_SOUND_ID);
    }
    chosen_ = *next;
    return;
  }
  if (key.code == MENU_CONFIRM_KEY && !confirmHeld_) {
    confirmHeld_ = true;
    choose();
  }
}

void HangarScreen::choose() {
  if (fitting_.has_value()) {
    return;
  }
  if (!settings_->settings().shipLocker.items.test(chosen_)) {
    return;
  }
  sounds_->play(CLICK_SOUND_ID);
  const ShipLook worn = settings_->settings().shipLook;
  if (wears(worn, chosen_)) {
    static_cast<void>(
        settings_->apply(SHIP_LOOK_SETTING, without(worn, chosen_)));
    return;
  }
  static_cast<void>(
      settings_->apply(SHIP_LOOK_SETTING, wearing(worn, chosen_)));
  if (motion_ == MotionPreference::Full) {
    fitting_ = FittingRun{.item = chosen_, .started = elapsed_, .before = worn};
  }
}

void HangarScreen::repaint() {
  const ShipLook worn = settings_->settings().shipLook;
  canvas_->clear();
  canvas_->write(HANGAR_LABEL, HANGAR_TITLE_ANCHOR, HANGAR_TITLE_STYLE);
  canvas_->fill(HANGAR_RULE, EDGE_BLUE);
  paintLocker(*canvas_, LockerView{.chosen = chosen_,
                                   .worn = worn,
                                   .locker = settings_->settings().shipLocker});
  paintDockWall(*canvas_, litBeaconOf(elapsed_, motion_));
  paintBlueprint(*canvas_, chosen_);
  paintDock(*canvas_, DockScene{.palette = palette_,
                                .shipColor = shipColor_,
                                .worn = worn,
                                .fitting = fitting_,
                                .elapsed = elapsed_,
                                .motion = motion_});
  canvas_->write(statusOf(chosen_, worn, fitting_.has_value(),
                          settings_->settings().shipLocker),
                 HANGAR_STATUS_ANCHOR, HANGAR_STATUS_STYLE);
  backChip_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
