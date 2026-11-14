#include "LobbyScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include "BarRowWidth.hpp"
#include "BoundAction.hpp"
#include "Button.hpp"
#include "ChatWindow.hpp"
#include "GameRules.hpp"
#include "GameRulesLine.hpp"
#include "GameScreenIdConstants.hpp"
#include "HangarBay.hpp"
#include "HangarBayConstants.hpp"
#include "HangarScreenIdConstants.hpp"
#include "HelpWalkthrough.hpp"
#include "HelpWalkthroughConstants.hpp"
#include "HelpWalkthroughParts.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "IShipLookExchange.hpp"
#include "ISoundPlayer.hpp"
#include "IWaitingRoom.hpp"
#include "InterfaceConstants.hpp"
#include "InterfacePixel.hpp"
#include "KeyChipParts.hpp"
#include "LobbyPartAreas.hpp"
#include "LobbyPlace.hpp"
#include "LobbyScreenConstants.hpp"
#include "LobbyScreenParts.hpp"
#include "LobbyState.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PictureScroll.hpp"
#include "PilotColor.hpp"
#include "PlayerLooks.hpp"
#include "PlayfieldMapping.hpp"
#include "RoomLayout.hpp"
#include "RoomLayouts.hpp"
#include "RowScrolling.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "ShiftedCanvas.hpp"
#include "ShipLook.hpp"
#include "SoundConstants.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

using identifiers::ShipLook;

namespace {

/** @returns The player of the waiting room with this number, or nullptr when
 * none has it. */
const network::LobbyPlayer* playerNumbered(
    std::span<const network::LobbyPlayer> players, std::uint8_t number) {
  const auto found =
      std::ranges::find(players, number, &network::LobbyPlayer::playerNumber);
  return found == players.end() ? nullptr : &*found;
}

/** @returns What the bay of a player shows: what the server lists, or what
 * the local player asked while the server lists another value. */
BayReadiness readinessOf(const network::LobbyPlayer& player,
                         std::optional<bool> requested) {
  if (requested.has_value() && *requested != player.ready) {
    return *requested ? BayReadiness::ReadyAsked : BayReadiness::NotReadyAsked;
  }
  return player.ready ? BayReadiness::Ready : BayReadiness::NotReady;
}

/** @returns The status line: none without players; who is not ready, by name
 * when a single pilot is not; or that every listed pilot is ready. */
std::string statusLine(std::span<const network::LobbyPlayer> players,
                       std::uint8_t ownNumber) {
  if (players.empty()) {
    return {};
  }
  const auto notReady =
      std::ranges::count(players, false, &network::LobbyPlayer::ready);
  if (notReady == 0) {
    return ALL_READY_STATUS;
  }
  if (notReady > 1) {
    return std::to_string(notReady) + PILOTS_NOT_READY_STATUS;
  }
  const auto waiting =
      std::ranges::find(players, false, &network::LobbyPlayer::ready);
  if (waiting->playerNumber == ownNumber) {
    return YOU_NOT_READY_STATUS;
  }
  return waiting->nickname + PILOT_NOT_READY_STATUS;
}

/** @returns Whether @p event presses (true) or releases (false) @p key;
 * nothing for any other event. */
std::optional<bool> readyKeyDownOf(const sf::Event& event,
                                   sf::Keyboard::Scancode key) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>();
      pressed != nullptr && pressed->scancode == key) {
    return true;
  }
  if (const auto* released = event.getIf<sf::Event::KeyReleased>();
      released != nullptr && released->scancode == key) {
    return false;
  }
  return std::nullopt;
}

/** @brief Tells each of @p widgets that the events stop reaching it, so none
 * waits for a release or stays drawn pressed. */
template <typename... Widgets>
void dropHeldInput(const PlayfieldMapping& pointAt, Widgets&... widgets) {
  (static_cast<void>(widgets.handleEvent(sf::Event::FocusLost{}, pointAt)),
   ...);
}

/** @returns The line of @p rules, written once when the room is built;
 * nothing when the client does not know the rules. */
std::optional<std::string> rulesLineIfKnown(
    const std::optional<network::GameRules>& rules) {
  if (!rules.has_value()) {
    return std::nullopt;
  }
  return rulesLineOf(*rules);
}

/** @returns Where the rules of the game are written: centered between the end
 * of the heading and the start of the count of @p pilots, both measured as
 * they are written, on the row of the heading. */
sf::Vector2i rulesAnchorOf(const IInterfaceCanvas& canvas,
                           const std::string& pilots) {
  const int headingEnd =
      SAFE_MARGIN + canvas.textWidth(LOBBY_HEADING, TextFont::Regular);
  const int pilotsStart =
      PILOT_COUNT_ANCHOR.x - canvas.textWidth(pilots, PILOT_COUNT_STYLE.font);
  return {(headingEnd + pilotsStart) / SIDES_PER_AXIS, GAME_RULES_TOP};
}

/** @returns How many pilots the waiting room lists, in words. */
std::string pilotCount(std::size_t count) {
  return std::to_string(count) +
         (count == 1 ? ONE_PILOT_TEXT : SEVERAL_PILOTS_TEXT);
}

/** @returns Where the bay of a slot starts: the bays stand side by side from
 * the left of the area, HANGAR_BAY_GAP apart. */
sf::Vector2i bayTopLeft(sf::IntRect area, int slot) {
  return area.position +
         sf::Vector2i(slot * (HANGAR_BAY_SIZE.x + HANGAR_BAY_GAP), 0);
}

/** @returns How large the four bays are side by side at the text size of
 * @p metrics: as wide as the rule, as tall as one bay. */
sf::Vector2i lobbyBaysSizeOf(TextMetrics metrics) {
  const sf::Vector2i bay = hangarBayLayoutOf(metrics).size;
  return {barRowWidth(LOBBY_BAY_COUNT, bay, HANGAR_BAY_GAP), bay.y};
}

/** @returns Where a chip with @p text lies: as MenuBuilder lays it ending at
 * @p right, on the row of chips whose top is @p top. */
sf::IntRect chipAt(const Button::Text& text, std::optional<int> right,
                   int top) {
  sf::IntRect area = MenuBuilder::chipArea(text, right);
  area.position.y = top;
  return area;
}

/** @returns What the chips of the leaderboard and of the hangar say, at the
 * text size of @p metrics. */
Button::Text leaderboardChipText(TextMetrics metrics) {
  return {.label = LEADERBOARD_LABEL,
          .keyName = LEADERBOARD_KEY_NAME,
          .size = metrics};
}

Button::Text hangarChipText(TextMetrics metrics) {
  return {.label = HANGAR_LABEL, .keyName = HANGAR_KEY_NAME, .size = metrics};
}

/** @returns Where the room paints BACK, its cancel chip, on the lower row of
 * its chips. */
sf::IntRect backChipArea(const RoomLayout& room, TextMetrics metrics) {
  return chipAt(
      {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME, .size = metrics},
      std::nullopt, room.lowerChipsTop);
}

/** @returns Where the room paints HELP, BUTTON_GAP to the left of BACK. */
sf::IntRect helpChipArea(const RoomLayout& room, TextMetrics metrics) {
  return chipAt(
      {.label = HELP_LABEL, .keyName = HELP_KEY_NAME, .size = metrics},
      backChipArea(room, metrics).position.x - BUTTON_GAP, room.lowerChipsTop);
}

/** @returns Where the room paints RANKING: BUTTON_GAP to the left of HELP
 * while the chips take one row; against the right side of the safe margin,
 * on the upper row, once they take two. */
sf::IntRect leaderboardChipArea(const RoomLayout& room, TextMetrics metrics) {
  const bool oneRow = room.upperChipsTop == room.lowerChipsTop;
  const std::optional<int> right =
      oneRow
          ? std::optional(helpChipArea(room, metrics).position.x - BUTTON_GAP)
          : std::nullopt;
  return chipAt(leaderboardChipText(metrics), right, room.upperChipsTop);
}

/** @returns Where the room paints HANGAR, BUTTON_GAP to the left of
 * RANKING, on its row. */
sf::IntRect hangarChipArea(const RoomLayout& room, TextMetrics metrics) {
  const sf::IntRect leaderboard = leaderboardChipArea(room, metrics);
  return chipAt(hangarChipText(metrics), leaderboard.position.x - BUTTON_GAP,
                leaderboard.position.y);
}

/** @returns How far the room is scrolled after @p event: moved by
 * ROOM_SCROLL_STEP a notch of the wheel, never past either end; at its
 * bottom once a key that moves the selection selects BACK, at its top once
 * it selects READY; unchanged by any other event. */
int roomScrollAfter(const sf::Event& event, const Menu& menu,
                    PictureScroll room) {
  if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>();
      wheel != nullptr && wheel->wheel == sf::Mouse::Wheel::Vertical) {
    const auto notches = static_cast<int>(std::lround(wheel->delta));
    return std::clamp(room.scroll - (notches * ROOM_SCROLL_STEP), 0, room.last);
  }
  const auto* key = event.getIf<sf::Event::KeyPressed>();
  if (key == nullptr || std::ranges::find(ROOM_SELECTION_KEYS, key->code) ==
                            ROOM_SELECTION_KEYS.end()) {
    return room.scroll;
  }
  return menu.selectedButton() == ROOM_BACK_BUTTON ? room.last : 0;
}

/** @returns Whether @p event presses the left button inside @p area, read on
 * the grid of the interface. */
bool clicksInside(const sf::Event& event, std::optional<sf::IntRect> area,
                  const PlayfieldMapping& playfieldPointAt) {
  const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
  return click != nullptr && click->button == sf::Mouse::Button::Left &&
         area.has_value() &&
         area->contains(interfacePixelAt(playfieldPointAt(click->position)));
}

/** @returns The slot of the bay of the local player, or nothing while the
 * server lists no player with its number. */
std::optional<int> ownSlotOf(std::span<const network::LobbyPlayer> players,
                             std::uint8_t ownNumber) {
  if (playerNumbered(players, ownNumber) == nullptr) {
    return std::nullopt;
  }
  return pilotColorOfPlayerNumber(ownNumber);
}

/** @returns The bay of slot @p ownSlot among @p bays: nothing without
 * bays or without a slot. */
std::optional<sf::IntRect> ownBayArea(std::optional<sf::IntRect> bays,
                                      std::optional<int> ownSlot) {
  if (!bays.has_value() || !ownSlot.has_value()) {
    return std::nullopt;
  }
  return sf::IntRect(bayTopLeft(*bays, *ownSlot),
                     {HANGAR_BAY_SIZE.x, bays->size.y});
}

/** @returns The bay of the local player in the room of @p menu: nothing while
 * the server lists no player with its number, or while the room shows no
 * bays. */
std::optional<sf::IntRect> ownBayAreaIn(const Menu& menu,
                                        const IWaitingRoom& waitingRoom,
                                        const IServerConnection& connection) {
  return ownBayArea(
      menu.illustrationArea(),
      ownSlotOf(waitingRoom.lobbyPlayers(), connection.status().playerNumber));
}

/** @brief What the room reads to place the parts its help explains: its
 * layout, its bays, the slot of the local player, how wide the status line
 * is written, and the text size. */
struct RoomParts {
  RoomLayout room;
  sf::IntRect bays;
  std::optional<int> ownSlot;
  int statusWidth{};
  TextMetrics metrics;
};

/** @returns Where the room paints the parts its help explains, as
 * MenuBuilder lays a room: the bays, where the menu lays its illustration;
 * READY, its only bar, centered where the layout puts the bars, as wide as
 * its longer label needs; the status line under READY, centered; then BACK.
 * Menu gives no area of a bar, so READY and the status line are placed by
 * the same arithmetic as MenuBuilder::placeRoom. */
LobbyPartAreas partAreasOf(const RoomParts& parts) {
  const int readyWidth = std::max(
      BUTTON_SIZE.x, regularTextWidth(parts.metrics, CANCEL_READY_LABEL) +
                         (SIDES_PER_AXIS * BUTTON_TEXT_OFFSET.x));
  const sf::IntRect ready(
      {(INTERFACE_AREA.size.x - readyWidth) / SIDES_PER_AXIS,
       parts.room.barsTop},
      {readyWidth, parts.room.barHeight});
  const int statusShown = std::max(parts.statusWidth, readyWidth);
  return {.bays = parts.bays,
          .ownBay = ownBayArea(parts.bays, parts.ownSlot).value_or(parts.bays),
          .ready = ready,
          .status = sf::IntRect(
              {(INTERFACE_AREA.size.x - statusShown) / SIDES_PER_AXIS,
               parts.room.statusTop},
              {statusShown, lineHeightOf(parts.metrics)}),
          .back = backChipArea(parts.room, parts.metrics)};
}

/** @returns The look @p looks shows on the player numbered @p number; nothing
 * worn when it does not list that player. */
ShipLook lookOf(std::span<const network::PlayerLook> looks,
                std::uint8_t number) {
  const ShipLook* const found = network::findLook(looks, number);
  return found == nullptr ? ShipLook{} : *found;
}

}  // namespace

LobbyScreen::LobbyScreen(LobbyScreenParts parts,
                         std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      connection_(&parts.connection.get()),
      waitingRoom_(&parts.waitingRoom.get()),
      shipLooks_(&parts.shipLooks.get()),
      sounds_(&parts.sounds.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      readyKey_(parts.keys.get().keyOf(BoundAction::Ready)),
      palette_(parts.palette),
      motion_(parts.motion),
      place_(parts.place),
      rulesLine_(rulesLineIfKnown(parts.rules)),
      room_(roomLayoutOf(lobbyBaysSizeOf(canvas_->textMetrics()).y,
                         canvas_->textMetrics())),
      menu_(buildMenu(parts.keyName(readyKey_))),
      help_(HelpWalkthroughParts{
          .sounds = parts.sounds,
          .keys = parts.keys,
          .keyName = parts.keyName,
          .palette = parts.palette,
          .motion = parts.motion,
          .chipArea = helpChipArea(room_, canvas_->textMetrics()),
          .metrics = canvas_->textMetrics()}),
      leaderboardChip_(KeyChipParts{
          .text = leaderboardChipText(canvas_->textMetrics()),
          .area = leaderboardChipArea(room_, canvas_->textMetrics()),
          .key = LEADERBOARD_KEY,
          .action =
              [this] {
                handleEvent(sf::Event::FocusLost{});
                navigator_->push(LEADERBOARD_SCREEN);
              },
          .sounds = parts.sounds}),
      look_(&parts.look.get()),
      hangarChip_(
          KeyChipParts{.text = hangarChipText(canvas_->textMetrics()),
                       .area = hangarChipArea(room_, canvas_->textMetrics()),
                       .key = HANGAR_KEY,
                       .action =
                           [this] {
                             handleEvent(sf::Event::FocusLost{});
                             navigator_->push(HANGAR_SCREEN);
                           },
                       .sounds = parts.sounds}),
      chat_(parts.chat.get(), parts.settings.get()) {
  repaint();
}

void LobbyScreen::handleEvent(const sf::Event& event) {
  const auto pointAt = shiftedMappingOf({0, scroll_}, playfieldPointAt_);
  if (!chat_.isOpen() && help_.handleEvent(event, pointAt)) {
    dropHeldInput(pointAt, leaderboardChip_, hangarChip_);
    return;
  }
  if (chat_.handleEvent(event)) {
    readyKeyHeld_ = false;
    dropHeldInput(pointAt, leaderboardChip_, hangarChip_, menu_);
    return;
  }
  if (const std::optional<bool> down = readyKeyDownOf(event, readyKey_)) {
    if (*down && !readyKeyHeld_) {
      sounds_->play(CLICK_SOUND_ID);
      toggleReady();
    }
    readyKeyHeld_ = *down;
    return;
  }
  readyKeyHeld_ = readyKeyHeld_ && !event.is<sf::Event::FocusLost>();
  if (leaderboardChip_.handleEvent(event, pointAt) ||
      hangarChip_.handleEvent(event, pointAt)) {
    return;
  }
  if (clicksInside(event, ownBayAreaIn(menu_, *waitingRoom_, *connection_),
                   pointAt)) {
    readyKeyHeld_ = false;
    dropHeldInput(pointAt, leaderboardChip_, hangarChip_, menu_);
    navigator_->push(HANGAR_SCREEN);
    return;
  }
  menu_.handleEvent(event, pointAt);
  scroll_ = roomScrollAfter(event, menu_,
                            {.scroll = scroll_, .last = lastScrollOf(room_)});
}

void LobbyScreen::update(engine::Duration elapsed) {
  menu_.advance(elapsed);
  help_.advance(elapsed);
  elapsed_ += elapsed;
  if (!gameAsked_ && waitingRoom_->gameStarted()) {
    gameAsked_ = true;
    navigator_->replaceAll(GAME_SCREEN);
  }
  shipLooks_->chooseLook(*look_);
  repaint();
}

void LobbyScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool LobbyScreen::hidesScreensBelow() const { return true; }

Menu LobbyScreen::buildMenu(std::string readyKeyName) {
  return MenuBuilder(MenuBuilder::Frame::Room, sounds_, canvas_->textMetrics())
      .heading(LOBBY_HEADING)
      .illustration(lobbyBaysSizeOf(canvas_->textMetrics()))
      .button({.label = CANCEL_READY_LABEL, .keyName = std::move(readyKeyName)},
              [this] { toggleReady(); })
      .cancelButton({.label = BACK_LABEL}, [this] { leave(); })
      .build();
}

void LobbyScreen::toggleReady() { waitingRoom_->setReady(!wantsReady()); }

void LobbyScreen::leave() {
  connection_->cancel();
  if (place_ == LobbyPlace::AfterGame) {
    navigator_->replaceAll(TITLE_SCREEN);
    return;
  }
  navigator_->pop();
}

bool LobbyScreen::wantsReady() const {
  const std::optional<bool> requested = waitingRoom_->requestedReady();
  if (requested.has_value()) {
    return *requested;
  }
  const network::LobbyPlayer* own = playerNumbered(
      waitingRoom_->lobbyPlayers(), connection_->status().playerNumber);
  return own != nullptr && own->ready;
}

void LobbyScreen::repaint() {
  const std::span<const network::LobbyPlayer> players =
      waitingRoom_->lobbyPlayers();
  const std::uint8_t ownNumber = connection_->status().playerNumber;
  std::string status = statusLine(players, ownNumber);
  const LobbyPartAreas parts = partAreasOf(
      {.room = room_,
       .bays = menu_.illustrationArea().value_or(sf::IntRect()),
       .ownSlot = ownSlotOf(players, ownNumber),
       .statusWidth = canvas_->textWidth(status, ROOM_STATUS_STYLE.font),
       .metrics = canvas_->textMetrics()});
  menu_.setLabel(READY_BUTTON, wantsReady() ? CANCEL_READY_LABEL : READY_LABEL);
  menu_.setStatus(std::move(status));
  canvas_->clear();
  ShiftedCanvas room(*canvas_, {0, -scroll_});
  if (!help_.showsGame()) {
    menu_.paint(room);
    paintBays(room);
    const std::string pilots = pilotCount(players.size());
    room.write(pilots, PILOT_COUNT_ANCHOR, PILOT_COUNT_STYLE);
    if (rulesLine_.has_value()) {
      room.write(*rulesLine_, rulesAnchorOf(room, pilots), GAME_RULES_STYLE);
    }
    leaderboardChip_.paint(room);
    hangarChip_.paint(room);
    paintPictureScrollMarks(*canvas_,
                            {.scroll = scroll_, .last = lastScrollOf(room_)});
  }
  help_.paint(room, parts);
  chat_.paint(*canvas_, palette_.get(), LOBBY_CHAT_BOTTOM);
  canvas_->display();
}

void LobbyScreen::paintBays(IInterfaceCanvas& canvas) const {
  const std::optional<sf::IntRect> area = menu_.illustrationArea();
  if (!area.has_value()) {
    return;
  }
  const BayMotion motion{.elapsed = elapsed_, .preference = motion_};
  for (int slot = 0; slot < LOBBY_BAY_COUNT; ++slot) {
    paintHangarBay(canvas, bayOf(*area, slot), motion);
  }
}

HangarBay LobbyScreen::bayOf(sf::IntRect area, int slot) const {
  const auto number = static_cast<std::uint8_t>(playerNumberOfPilotColor(slot));
  HangarBay bay{.topLeft = bayTopLeft(area, slot),
                .playerNumber = number,
                .color = slot,
                .palette = palette_,
                .pilot = std::nullopt};
  const network::LobbyPlayer* player =
      playerNumbered(waitingRoom_->lobbyPlayers(), number);
  if (player != nullptr) {
    bay.pilot = pilotOf(*player);
  }
  return bay;
}

BayPilot LobbyScreen::pilotOf(const network::LobbyPlayer& player) const {
  const bool isLocalPlayer =
      player.playerNumber == connection_->status().playerNumber;
  const std::optional<bool> requested =
      isLocalPlayer ? waitingRoom_->requestedReady() : std::nullopt;
  return {.nickname = player.nickname,
          .readiness = readinessOf(player, requested),
          .isLocalPlayer = isLocalPlayer,
          .look = isLocalPlayer
                      ? *look_
                      : lookOf(shipLooks_->playerLooks(), player.playerNumber)};
}

}  // namespace rtype::client
