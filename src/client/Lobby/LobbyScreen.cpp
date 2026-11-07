#include "LobbyScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include "BoundAction.hpp"
#include "ChatWindow.hpp"
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
#include "PilotColor.hpp"
#include "PlayerLooks.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "ShipLook.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"

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

/** @returns Where the room paints BACK, its cancel chip. */
sf::IntRect backChipArea() {
  return MenuBuilder::chipArea(
      {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME});
}

/** @returns Where the room paints HELP, BUTTON_GAP to the left of BACK. */
sf::IntRect helpChipArea() {
  return MenuBuilder::chipArea({.label = HELP_LABEL, .keyName = HELP_KEY_NAME},
                               backChipArea().position.x - BUTTON_GAP);
}

/** @returns Where the room paints RANKING, BUTTON_GAP to the left of HELP. */
sf::IntRect leaderboardChipArea() {
  return MenuBuilder::chipArea(
      {.label = LEADERBOARD_LABEL, .keyName = LEADERBOARD_KEY_NAME},
      helpChipArea().position.x - BUTTON_GAP);
}

/** @returns Where the room paints HANGAR, BUTTON_GAP to the left of
 * RANKING. */
sf::IntRect hangarChipArea() {
  return MenuBuilder::chipArea(
      {.label = HANGAR_LABEL, .keyName = HANGAR_KEY_NAME},
      leaderboardChipArea().position.x - BUTTON_GAP);
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
  return sf::IntRect(bayTopLeft(*bays, *ownSlot), HANGAR_BAY_SIZE);
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

/** @returns Where the room paints the parts its help explains, as
 * MenuBuilder lays a room: the bays, where the menu lays its illustration;
 * READY, its only bar, centered SPACE_ABOVE_ROOM_BARS under them; the status
 * line BUTTON_GAP under READY, centered; then BACK. Menu gives no area of a
 * bar, so READY and the status line are placed by the same arithmetic as
 * MenuBuilder::placeRoom. */
LobbyPartAreas partAreasOf(sf::IntRect bays, std::optional<int> ownSlot,
                           int statusWidth) {
  const sf::IntRect ready(
      {(INTERFACE_AREA.size.x - BUTTON_SIZE.x) / SIDES_PER_AXIS,
       bays.position.y + bays.size.y + SPACE_ABOVE_ROOM_BARS},
      BUTTON_SIZE);
  const int statusShown = std::max(statusWidth, BUTTON_SIZE.x);
  return {.bays = bays,
          .ownBay = ownBayArea(bays, ownSlot).value_or(bays),
          .ready = ready,
          .status = sf::IntRect(
              {(INTERFACE_AREA.size.x - statusShown) / SIDES_PER_AXIS,
               ready.position.y + ready.size.y + BUTTON_GAP},
              {statusShown, TEXT_LINE_HEIGHT}),
          .back = backChipArea()};
}

/** @returns The look @p looks shows on the player numbered @p number; nothing
 * worn when it does not list that player. */
ShipLook lookOf(std::span<const network::PlayerLook> looks,
                std::uint8_t number) {
  const auto found =
      std::ranges::find(looks, number, &network::PlayerLook::playerNumber);
  return found == looks.end() ? ShipLook{} : found->look;
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
      menu_(buildMenu(parts.keyName(readyKey_))),
      help_(HelpWalkthroughParts{.sounds = parts.sounds,
                                 .keys = parts.keys,
                                 .keyName = parts.keyName,
                                 .palette = parts.palette,
                                 .motion = parts.motion,
                                 .chipArea = helpChipArea()}),
      leaderboardChip_(KeyChipParts{
          .text = {.label = LEADERBOARD_LABEL, .keyName = LEADERBOARD_KEY_NAME},
          .area = leaderboardChipArea(),
          .key = LEADERBOARD_KEY,
          .action =
              [this] {
                handleEvent(sf::Event::FocusLost{});
                navigator_->push(LEADERBOARD_SCREEN);
              },
          .sounds = parts.sounds}),
      look_(&parts.look.get()),
      hangarChip_(KeyChipParts{
          .text = {.label = HANGAR_LABEL, .keyName = HANGAR_KEY_NAME},
          .area = hangarChipArea(),
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
  if (!chat_.isOpen() && help_.handleEvent(event, playfieldPointAt_)) {
    leaderboardChip_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
    hangarChip_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
    return;
  }
  if (chat_.handleEvent(event)) {
    readyKeyHeld_ = false;
    leaderboardChip_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
    hangarChip_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
    menu_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
    return;
  }
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>();
      pressed != nullptr && pressed->scancode == readyKey_) {
    if (!readyKeyHeld_) {
      readyKeyHeld_ = true;
      sounds_->play(CLICK_SOUND_ID);
      toggleReady();
    }
    return;
  }
  if (const auto* released = event.getIf<sf::Event::KeyReleased>();
      released != nullptr && released->scancode == readyKey_) {
    readyKeyHeld_ = false;
    return;
  }
  readyKeyHeld_ = readyKeyHeld_ && !event.is<sf::Event::FocusLost>();
  if (leaderboardChip_.handleEvent(event, playfieldPointAt_) ||
      hangarChip_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  if (clicksInside(event, ownBayAreaIn(menu_, *waitingRoom_, *connection_),
                   playfieldPointAt_)) {
    readyKeyHeld_ = false;
    leaderboardChip_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
    hangarChip_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
    menu_.handleEvent(sf::Event::FocusLost{}, playfieldPointAt_);
    navigator_->push(HANGAR_SCREEN);
    return;
  }
  menu_.handleEvent(event, playfieldPointAt_);
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
  return MenuBuilder(MenuBuilder::Frame::Room, sounds_)
      .heading(LOBBY_HEADING)
      .illustration(LOBBY_BAYS_SIZE)
      .button({.label = READY_LABEL, .keyName = std::move(readyKeyName)},
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
  const LobbyPartAreas parts =
      partAreasOf(menu_.illustrationArea().value_or(sf::IntRect()),
                  ownSlotOf(players, ownNumber),
                  canvas_->textWidth(status, ROOM_STATUS_STYLE.font));
  menu_.setLabel(READY_BUTTON, wantsReady() ? CANCEL_READY_LABEL : READY_LABEL);
  menu_.setStatus(std::move(status));
  canvas_->clear();
  if (!help_.showsGame()) {
    menu_.paint(*canvas_);
    paintBays();
    canvas_->write(pilotCount(players.size()), PILOT_COUNT_ANCHOR,
                   PILOT_COUNT_STYLE);
    leaderboardChip_.paint(*canvas_);
    hangarChip_.paint(*canvas_);
  }
  help_.paint(*canvas_, parts);
  chat_.paint(*canvas_, palette_.get(), LOBBY_CHAT_BOTTOM);
  canvas_->display();
}

void LobbyScreen::paintBays() {
  const std::optional<sf::IntRect> area = menu_.illustrationArea();
  if (!area.has_value()) {
    return;
  }
  const BayMotion motion{.elapsed = elapsed_, .preference = motion_};
  for (int slot = 0; slot < LOBBY_BAY_COUNT; ++slot) {
    paintHangarBay(*canvas_, bayOf(*area, slot), motion);
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
