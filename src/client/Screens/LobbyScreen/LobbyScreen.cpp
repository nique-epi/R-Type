#include "LobbyScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include "HangarBay.hpp"
#include "HangarBayConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "LobbyPlace.hpp"
#include "LobbyScreenConstants.hpp"
#include "LobbyScreenParts.hpp"
#include "LobbyState.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PilotColor.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenId.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

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

}  // namespace

LobbyScreen::LobbyScreen(LobbyScreenParts parts,
                         std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      connection_(&parts.connection.get()),
      sounds_(&parts.sounds.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      readyKey_(parts.readyKey.key),
      palette_(parts.palette),
      motion_(parts.motion),
      place_(parts.place),
      menu_(buildMenu(std::move(parts.readyKey.name))) {
  repaint();
}

void LobbyScreen::handleEvent(const sf::Event& event) {
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
  if (event.is<sf::Event::FocusLost>()) {
    readyKeyHeld_ = false;
  }
  menu_.handleEvent(event, playfieldPointAt_);
}

void LobbyScreen::update(engine::Duration elapsed) {
  menu_.advance(elapsed);
  elapsed_ += elapsed;
  if (!gameAsked_ && connection_->gameStarted()) {
    gameAsked_ = true;
    navigator_->replaceAll(ScreenId::Game);
  }
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

void LobbyScreen::toggleReady() { connection_->setReady(!wantsReady()); }

void LobbyScreen::leave() {
  connection_->cancel();
  if (place_ == LobbyPlace::AfterGame) {
    navigator_->replaceAll(ScreenId::Title);
    return;
  }
  navigator_->pop();
}

bool LobbyScreen::wantsReady() const {
  const std::optional<bool> requested = connection_->requestedReady();
  if (requested.has_value()) {
    return *requested;
  }
  const network::LobbyPlayer* own = playerNumbered(
      connection_->lobbyPlayers(), connection_->status().playerNumber);
  return own != nullptr && own->ready;
}

void LobbyScreen::repaint() {
  const std::span<const network::LobbyPlayer> players =
      connection_->lobbyPlayers();
  const std::uint8_t ownNumber = connection_->status().playerNumber;
  menu_.setLabel(READY_BUTTON, wantsReady() ? CANCEL_READY_LABEL : READY_LABEL);
  menu_.setStatus(statusLine(players, ownNumber));
  canvas_->clear();
  menu_.paint(*canvas_);
  paintBays();
  canvas_->write(pilotCount(players.size()), PILOT_COUNT_ANCHOR,
                 PILOT_COUNT_STYLE);
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
      playerNumbered(connection_->lobbyPlayers(), number);
  if (player != nullptr) {
    bay.pilot = pilotOf(*player);
  }
  return bay;
}

BayPilot LobbyScreen::pilotOf(const network::LobbyPlayer& player) const {
  const bool isLocalPlayer =
      player.playerNumber == connection_->status().playerNumber;
  const std::optional<bool> requested =
      isLocalPlayer ? connection_->requestedReady() : std::nullopt;
  return {.nickname = player.nickname,
          .readiness = readinessOf(player, requested),
          .isLocalPlayer = isLocalPlayer};
}

}  // namespace rtype::client
