#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <functional>
#include <memory>
#include <string>
#include "HelpWalkthrough.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "IWaitingRoom.hpp"
#include "LobbyPlace.hpp"
#include "LobbyScreenParts.hpp"
#include "LobbyState.hpp"
#include "Menu.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

struct BayPilot;
struct HangarBay;

/**
 * @brief The waiting room, pushed over the connection screen once the server
 * accepted the player: a bay per player number, each player the server lists
 * in the bay of its number, the local player tagged YOU; then READY, the
 * status line under it, and BACK as a chip in the corner.
 *
 * READY, or the key bound to BoundAction::Ready from anywhere on the screen,
 * asks the server to set the player ready, or not ready once asked: the
 * connection sends a value, never a toggle, and the key held down asks once.
 * The bar reads CANCEL READY once the player asked to be ready. A bay shows
 * what the server lists; the bay of the local player shows, in gold, what the
 * player asked while the server lists another value. The status line names
 * who is not ready: a game starts as soon as every listed pilot is, however
 * many they are.
 *
 * Once the connection learns that the game started, the screen asks for the
 * game in place of every screen, once. BACK, or MENU_CANCEL_KEY, leaves the
 * server and closes the screen, back to the connection screen; after a game,
 * it opens the title in place of every screen instead. The key of READY wins
 * over the keys of the menu.
 *
 * HELP, a chip beside BACK, or HELP_KEY opens the help of the room
 * (HelpWalkthrough) over it: while it is open it takes every event but the
 * release of a key, the room keeps updating under it, and the game, once it
 * starts, replaces both.
 */
class LobbyScreen : public IScreen {
 public:
  /**
   * @param canvas Picture the waiting room is painted on, never null: the
   * screen owns it, so it comes apart from the parts it borrows.
   */
  LobbyScreen(LobbyScreenParts parts, std::unique_ptr<IInterfaceCanvas> canvas);

  /** @brief The actions of the menu point at the screen, so it never moves. */
  LobbyScreen(const LobbyScreen&) = delete;
  LobbyScreen& operator=(const LobbyScreen&) = delete;
  LobbyScreen(LobbyScreen&&) = delete;
  LobbyScreen& operator=(LobbyScreen&&) = delete;
  ~LobbyScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  [[nodiscard]] Menu buildMenu(std::string readyKeyName);

  /** @brief Asks the server for the other ready state: not ready once the
   * player wants to be ready, ready otherwise. */
  void toggleReady();

  /** @brief Leaves the server, then asks for the screen to be closed. */
  void leave();

  /** @returns The ready state the player asked for; before any request, the
   * one the server lists for the player. */
  [[nodiscard]] bool wantsReady() const;

  /** @brief Paints the menu, the bays and the number of pilots, read from the
   * connection once per update. */
  void repaint();

  /** @brief Paints one bay per player number, side by side in the
   * illustration area of the menu. */
  void paintBays();

  /** @returns The bay of a slot, 0 to LOBBY_BAY_COUNT - 1: where it lies in
   * the area of the bays, its player number, the color of its ship, and the
   * pilot the server lists with that number, none while the bay is open. */
  [[nodiscard]] HangarBay bayOf(sf::IntRect area, int slot) const;

  /** @returns The pilot of a bay as the server lists it; for the local player,
   * the ready state asked for shows while the server lists another. */
  [[nodiscard]] BayPilot pilotOf(const network::LobbyPlayer& player) const;

  IScreenNavigator* navigator_{};
  IServerConnection* connection_{};
  IWaitingRoom* waitingRoom_{};
  ISoundPlayer* sounds_{};
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  sf::Keyboard::Scancode readyKey_{};
  std::reference_wrapper<const PlayerPalette> palette_;
  MotionPreference motion_{};
  LobbyPlace place_{};
  engine::Duration elapsed_{};
  bool readyKeyHeld_ = false;
  bool gameAsked_ = false;
  Menu menu_;
  HelpWalkthrough help_;
};

}  // namespace rtype::client
