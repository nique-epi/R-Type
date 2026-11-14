#include "DestinationScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <utility>
#include "BarState.hpp"
#include "CutCornerBox.hpp"
#include "DestinationCard.hpp"
#include "DestinationScreenConstants.hpp"
#include "DestinationScreenParts.hpp"
#include "DestinationSpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "InterfacePixel.hpp"
#include "KeyChipParts.hpp"
#include "KopecksLabelConstants.hpp"
#include "KopecksPainting.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlayStart.hpp"
#include "PlayerSettings.hpp"
#include "RenderingConstants.hpp"
#include "SoundConstants.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The card SERVER: it leads where PLAY led, the connection to the
 * server given at launch, or the server map. */
DestinationCard serverCardOf(PlayStart playStart) {
  return DestinationCard{.label = SERVER_LABEL,
                         .description = SERVER_DESCRIPTION,
                         .sheet = DESTINATION_SHEET_ID,
                         .frame = DESTINATION_PLANET_FRAME,
                         .screen = playStart == PlayStart::GivenServer
                                       ? CONNECTION_TO_CHOSEN_SERVER_SCREEN
                                       : SERVER_MAP_SCREEN};
}

sf::IntRect goChipArea() {
  return MenuBuilder::chipArea(
      {.label = GO_LABEL, .keyName = CONFIRM_KEY_NAME});
}

sf::IntRect backChipArea() {
  return MenuBuilder::chipArea(
      {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME},
      goChipArea().position.x - BUTTON_GAP);
}

/** @returns The index in the cards of the card standing in @p slot while
 * the card @p chosen is in the middle. */
std::size_t cardInSlot(std::size_t chosen, std::size_t slot) {
  return (chosen + slot + DESTINATION_COUNT - CHOSEN_CARD_SLOT) %
         DESTINATION_COUNT;
}

/** @returns The slot of the card under @p pixel, if any. */
std::optional<std::size_t> slotAt(sf::Vector2i pixel) {
  for (std::size_t slot = 0; slot < DESTINATION_CARD_AREAS.size(); ++slot) {
    if (DESTINATION_CARD_AREAS.at(slot).contains(pixel)) {
      return slot;
    }
  }
  return std::nullopt;
}

/** @returns Where the sprite of @p card lands, centered across @p card area
 * at @p top, enlarged by the whole factor that fits DESTINATION_SPRITE_BOX. */
ImagePlacement spritePlacementOf(const DestinationCard& card, sf::IntRect area,
                                 int top) {
  const int scale =
      DESTINATION_SPRITE_BOX / std::max(card.frame.width, card.frame.height);
  const sf::Vector2i size(card.frame.width * scale, card.frame.height * scale);
  return ImagePlacement{
      .topLeft = {area.position.x + ((area.size.x - size.x) / HALF_DIVISOR),
                  top + ((DESTINATION_SPRITE_BOX - size.y) / HALF_DIVISOR)},
      .scale = scale};
}

void paintSprite(IInterfaceCanvas& canvas, const DestinationCard& card,
                 ImagePlacement placement) {
  canvas.drawImage(
      TextureArea{.assetId = card.sheet, .rectangle = rectangleOf(card.frame)},
      placement, INTERFACE_AREA);
}

/** @brief Paints the card in the middle: its two frames, its sprite, its
 * name twice the size of a line. */
void paintChosenCard(IInterfaceCanvas& canvas, const DestinationCard& card) {
  const sf::IntRect area = DESTINATION_CARD_AREAS.at(CHOSEN_CARD_SLOT);
  const sf::Vector2i margin(CHOSEN_FRAME_MARGIN, CHOSEN_FRAME_MARGIN);
  CutCornerBox(
      sf::IntRect(area.position - margin, area.size + (margin * HALF_DIVISOR)),
      CHOSEN_FRAME_COLORS)
      .paint(canvas);
  CutCornerBox(area, CHOSEN_CARD_COLORS).paint(canvas);
  paintSprite(
      canvas, card,
      spritePlacementOf(card, area, area.position.y + CHOSEN_SPRITE_TOP));
  canvas.write(card.label,
               {area.position.x + (area.size.x / HALF_DIVISOR),
                area.position.y + CHOSEN_LABEL_TOP},
               CHOSEN_LABEL_STYLE);
}

/** @brief Paints a card at a side, in @p slot: its box, its sprite dimmed,
 * its name. */
void paintSideCard(IInterfaceCanvas& canvas, const DestinationCard& card,
                   std::size_t slot) {
  const sf::IntRect area = DESTINATION_CARD_AREAS.at(slot);
  CutCornerBox(area, SIDE_CARD_COLORS).paint(canvas);
  ImagePlacement placement =
      spritePlacementOf(card, area, area.position.y + SIDE_SPRITE_TOP);
  placement.tint = SIDE_SPRITE_TINT;
  paintSprite(canvas, card, placement);
  canvas.write(card.label,
               {area.position.x + (area.size.x / HALF_DIVISOR),
                area.position.y + area.size.y - SIDE_LABEL_FROM_BOTTOM},
               SIDE_LABEL_STYLE);
}

/** @brief Writes the marks that say the stand turns, outside the cards at
 * the sides. */
void paintTurnMarks(IInterfaceCanvas& canvas) {
  const sf::IntRect left = DESTINATION_CARD_AREAS.front();
  const sf::IntRect right = DESTINATION_CARD_AREAS.back();
  canvas.write(TURN_LEFT_MARK, {left.position.x - TURN_MARK_GAP, TURN_MARK_TOP},
               TURN_MARK_STYLE);
  canvas.write(TURN_RIGHT_MARK,
               {right.position.x + right.size.x + TURN_MARK_GAP, TURN_MARK_TOP},
               TURN_MARK_STYLE);
}

}  // namespace

DestinationScreen::DestinationScreen(DestinationScreenParts parts,
                                     std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      settings_(&parts.settings.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      cards_{serverCardOf(parts.playStart), SHOP_CARD, HANGAR_CARD},
      goChip_(
          KeyChipParts{.text = {.label = GO_LABEL, .keyName = CONFIRM_KEY_NAME},
                       .area = goChipArea(),
                       .key = MENU_CONFIRM_KEY,
                       .action = [this] { openChosen(); },
                       .sounds = parts.sounds,
                       .restState = BarState::Selected}),
      backChip_(KeyChipParts{
          .text = {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME},
          .area = backChipArea(),
          .key = MENU_CANCEL_KEY,
          .action = [this] { navigator_->pop(); },
          .sounds = parts.sounds}) {
  repaint();
}

void DestinationScreen::handleEvent(const sf::Event& event) {
  repaintDue_ = true;
  if (goChip_.handleEvent(event, playfieldPointAt_) ||
      backChip_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    if (pressed->code == TURN_LEFT_KEY) {
      turnBy(DESTINATION_COUNT - 1);
    } else if (pressed->code == TURN_RIGHT_KEY) {
      turnBy(1);
    }
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    pointerPressed(interfacePixelAt(playfieldPointAt_(click->position)));
  }
}

void DestinationScreen::update(engine::Duration /*elapsed*/) {
  if (repaintDue_ || settings_->kopecks != paintedKopecks_) {
    repaint();
  }
}

void DestinationScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool DestinationScreen::hidesScreensBelow() const { return true; }

void DestinationScreen::turnBy(std::size_t steps) {
  chosen_ = (chosen_ + steps) % DESTINATION_COUNT;
}

void DestinationScreen::openChosen() {
  navigator_->push(cards_.at(chosen_).screen);
}

void DestinationScreen::pointerPressed(sf::Vector2i pixel) {
  const std::optional<std::size_t> slot = slotAt(pixel);
  if (!slot.has_value()) {
    return;
  }
  if (*slot != CHOSEN_CARD_SLOT) {
    turnBy(*slot + DESTINATION_COUNT - CHOSEN_CARD_SLOT);
    return;
  }
  goChip_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
  backChip_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
  sounds_->play(CLICK_SOUND_ID);
  openChosen();
}

void DestinationScreen::repaint() {
  repaintDue_ = false;
  paintedKopecks_ = settings_->kopecks;
  canvas_->clear();
  canvas_->write(DESTINATION_TITLE, DESTINATION_TITLE_ANCHOR,
                 DESTINATION_TITLE_STYLE);
  canvas_->fill(DESTINATION_RULE, RULE_COLOR);
  paintKopecksBalance(*canvas_, KOPECKS_BALANCE_TOP_RIGHT, settings_->kopecks);
  for (std::size_t slot = 0; slot < DESTINATION_CARD_AREAS.size(); ++slot) {
    const DestinationCard& card = cards_.at(cardInSlot(chosen_, slot));
    if (slot == CHOSEN_CARD_SLOT) {
      paintChosenCard(*canvas_, card);
    } else {
      paintSideCard(*canvas_, card, slot);
    }
  }
  paintTurnMarks(*canvas_);
  canvas_->write(cards_.at(chosen_).description, DESTINATION_DESCRIPTION_ANCHOR,
                 DESTINATION_DESCRIPTION_STYLE);
  goChip_.paint(*canvas_);
  backChip_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
