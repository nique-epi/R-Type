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
#include "Ambience.hpp"
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
#include "LockerConstants.hpp"
#include "LockerPainting.hpp"
#include "LockerView.hpp"
#include "MenuBuilder.hpp"
#include "MotionPreference.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "RowWindow.hpp"
#include "SettingDefinitionConstants.hpp"
#include "ShipLocker.hpp"
#include "ShipLook.hpp"
#include "ShipLookNumbers.hpp"
#include "ShipLooks.hpp"
#include "SoundConstants.hpp"
#include "TextMetrics.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

using identifiers::ShipLook;

namespace {

/** @returns The item whose cell lies under @p position of the window, the
 * locker showing its rows from @p firstRow. */
std::optional<std::size_t> itemUnder(sf::Vector2i position,
                                     const PlayfieldMapping& playfieldPointAt,
                                     std::size_t firstRow) {
  return lockerItemAt(interfacePixelAt(playfieldPointAt(position)), firstRow);
}

/** @returns What BACK says, at the text size of @p metrics. */
Button::Text backText(TextMetrics metrics) {
  return {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME, .size = metrics};
}

/** @returns The status line: how to get @p item when the locker does not
 * hold it, else its name, then what Enter does with it, or that an arm is
 * fitting an item. */
std::string statusOf(std::size_t item, const PlayerSettings& settings,
                     bool fitting) {
  if (!settings.shipLocker.items.test(item)) {
    return identifiers::isHullItem(item) ? LOCKED_HULL_STATUS : LOCKED_STATUS;
  }
  const std::string status(lookItemLabelOf(item));
  if (fitting) {
    return status + FITTING_HINT;
  }
  return status + (wears(settings.shipLook, item) ? TAKE_OFF_HINT : WEAR_HINT);
}

}  // namespace

HangarScreen::HangarScreen(HangarScreenParts parts,
                           std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      ambience_(parts.audio.get(), Ambience::Hangar),
      sounds_(&parts.sounds.get()),
      settings_(&parts.settings.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      palette_(parts.palette),
      shipColor_(parts.shipColor),
      motion_(parts.motion),
      backChip_(KeyChipParts{
          .text = backText(canvas_->textMetrics()),
          .area = MenuBuilder::chipArea(backText(canvas_->textMetrics())),
          .key = MENU_CANCEL_KEY,
          .action = [this] { navigator_->pop(); },
          .sounds = parts.sounds}),
      rows_(LOCKER_ROWS_AT_OPENING) {
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
    chosen_ = itemUnder(moved->position, playfieldPointAt_, rows_.first)
                  .value_or(chosen_);
  } else if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
    scrolled(*wheel);
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    if (const std::optional<std::size_t> item =
            itemUnder(click->position, playfieldPointAt_, rows_.first)) {
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
    rows_ = rowsShowing(rows_, lockerRowOf(chosen_));
    return;
  }
  if (key.code == MENU_CONFIRM_KEY && !confirmHeld_) {
    confirmHeld_ = true;
    choose();
  }
}

void HangarScreen::scrolled(const sf::Event::MouseWheelScrolled& wheel) {
  if (wheel.wheel != sf::Mouse::Wheel::Vertical) {
    return;
  }
  rows_ = rowsScrolledBy(rows_, wheel.delta > 0.0F ? -1 : 1);
  chosen_ = itemUnder(wheel.position, playfieldPointAt_, rows_.first)
                .value_or(lockerItemShownIn(chosen_, rows_));
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
  if (motion_ == MotionPreference::Full && !identifiers::isHullItem(chosen_)) {
    fitting_ = FittingRun{.item = chosen_, .started = elapsed_, .before = worn};
    sounds_->play(ROBOT_ARM_SOUND_ID);
  }
}

void HangarScreen::repaint() {
  const ShipLook worn = settings_->settings().shipLook;
  const ShipLocker& locker = settings_->settings().shipLocker;
  canvas_->clear();
  canvas_->write(HANGAR_LABEL, HANGAR_TITLE_ANCHOR, HANGAR_TITLE_STYLE);
  canvas_->fill(HANGAR_RULE, EDGE_BLUE);
  paintLocker(
      *canvas_,
      LockerView{
          .chosen = chosen_, .worn = worn, .locker = locker, .rows = rows_});
  paintDockWall(*canvas_, litBeaconOf(elapsed_, motion_));
  paintBlueprint(*canvas_, chosen_);
  paintDock(*canvas_, DockScene{.palette = palette_,
                                .shipColor = shipColor_,
                                .worn = worn,
                                .fitting = fitting_,
                                .elapsed = elapsed_,
                                .motion = motion_});
  canvas_->write(statusOf(chosen_, settings_->settings(), fitting_.has_value()),
                 HANGAR_STATUS_ANCHOR, HANGAR_STATUS_STYLE);
  backChip_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
