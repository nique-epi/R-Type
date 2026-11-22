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
#include "IAccountSession.hpp"
#include "IInterfaceCanvas.hpp"
#include "IWallet.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfacePixel.hpp"
#include "KeyChipParts.hpp"
#include "Language.hpp"
#include "LockerChoice.hpp"
#include "LockerConstants.hpp"
#include "LockerPainting.hpp"
#include "LockerView.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "MotionPreference.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "RowWindow.hpp"
#include "SettingDefinitionConstants.hpp"
#include "ShipLocker.hpp"
#include "ShipLook.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookNumbers.hpp"
#include "ShipLooks.hpp"
#include "SoundConstants.hpp"
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

Button::Text backText(Language language) {
  return {.label = std::string(BACK_LABEL.in(language)),
          .keyName = CANCEL_KEY_NAME};
}

/** @returns The status line of the chosen item of @p view: how to get it
 * when the locker does not hold it, else its name, then what Enter does with
 * it, or that an arm is fitting an item. */
std::string statusOf(const LockerView& view, bool fitting, Language language) {
  if (!view.locker.items.test(view.chosen)) {
    return std::string((identifiers::isHullItem(view.chosen)
                            ? LOCKED_HULL_STATUS
                            : LOCKED_STATUS)
                           .in(language));
  }
  const std::string status(lookItemLabelOf(view.chosen, language));
  if (fitting) {
    return status + std::string(FITTING_HINT.in(language));
  }
  return status +
         std::string((wears(view.worn, view.chosen) ? TAKE_OFF_HINT : WEAR_HINT)
                         .in(language));
}

/** @returns The looks the player may wear, once known: the starter kit for a
 * guest, the looks the master read for an account; nothing until it read
 * them. */
std::optional<ShipLocker> knownLockerOf(const IAccountSession& account,
                                        const IWallet& wallet) {
  if (!account.signedIn()) {
    return ShipLocker{};
  }
  return wallet.ownedLooks();
}

/** @returns @p worn without every item @p locker does not hold. */
ShipLook ownedPartOf(ShipLook worn, const ShipLocker& locker) {
  for (std::size_t item = 0; item < identifiers::SHIP_LOOK_ITEM_COUNT; ++item) {
    if (!locker.items.test(item) && wears(worn, item)) {
      worn = without(worn, item);
    }
  }
  return worn;
}

}  // namespace

HangarScreen::HangarScreen(HangarScreenParts parts,
                           std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      ambience_(parts.audio.get(), Ambience::Hangar),
      sounds_(&parts.sounds.get()),
      settings_(&parts.settings.get()),
      account_(&parts.account.get()),
      wallet_(&parts.wallet.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      palette_(parts.palette),
      shipColor_(parts.shipColor),
      motion_(parts.motion),
      backChip_(KeyChipParts{
          .text = backText(canvas_->language()),
          .area = MenuBuilder::chipArea(backText(canvas_->language())),
          .key = MENU_CANCEL_KEY,
          .action = [this] { navigator_->pop(); },
          .sounds = parts.sounds}),
      rows_(LOCKER_ROWS_AT_OPENING) {
  wallet_->askWallet();
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
  takeOffWhatIsNotOwned();
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
  if (!knownLockerOf(*account_, *wallet_)
           .value_or(ShipLocker{})
           .items.test(chosen_)) {
    if (!account_->signedIn()) {
      confirmHeld_ = false;
      navigator_->push(SIGN_IN_NEEDED_SCREEN);
    }
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

void HangarScreen::takeOffWhatIsNotOwned() {
  const std::optional<ShipLocker> locker = knownLockerOf(*account_, *wallet_);
  if (!locker.has_value() || fitting_.has_value()) {
    return;
  }
  const ShipLook worn = settings_->settings().shipLook;
  const ShipLook owned = ownedPartOf(worn, *locker);
  if (owned != worn) {
    static_cast<void>(settings_->apply(SHIP_LOOK_SETTING, owned));
  }
}

void HangarScreen::repaint() {
  const ShipLook worn = settings_->settings().shipLook;
  const ShipLocker locker =
      knownLockerOf(*account_, *wallet_).value_or(ShipLocker{});
  const LockerView view{
      .chosen = chosen_, .worn = worn, .locker = locker, .rows = rows_};
  canvas_->clear();
  canvas_->write(HANGAR_LABEL.in(canvas_->language()), HANGAR_TITLE_ANCHOR,
                 HANGAR_TITLE_STYLE);
  canvas_->fill(HANGAR_RULE, EDGE_BLUE);
  paintLocker(*canvas_, view);
  paintDockWall(*canvas_, litBeaconOf(elapsed_, motion_));
  paintBlueprint(*canvas_, chosen_);
  paintDock(*canvas_, DockScene{.palette = palette_,
                                .shipColor = shipColor_,
                                .worn = worn,
                                .fitting = fitting_,
                                .elapsed = elapsed_,
                                .motion = motion_});
  canvas_->write(statusOf(view, fitting_.has_value(), canvas_->language()),
                 HANGAR_STATUS_ANCHOR, HANGAR_STATUS_STYLE);
  backChip_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
