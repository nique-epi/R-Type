#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "KeyBindings.hpp"
#include "KeyNames.hpp"
#include "LobbyState.hpp"
#include "Menu.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

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
 * server and closes the screen, back to the connection screen. The key of
 * READY wins over the keys of the menu.
 */
class LobbyScreen : public IScreen {
 public:
  /**
   * @param navigator Screens the waiting room lives among, which must outlive
   * it.
   * @param connection Session the waiting room reads and drives, which must
   * outlive the screen.
   * @param canvas Picture the waiting room is painted on, never null.
   * @param playfieldPointAt Gives the point of the playfield under a pixel of
   * the window.
   * @param keys Bindings of the player: the key of BoundAction::Ready is read
   * once, when the screen is built.
   * @param keyNaming Gives the name the keycap of READY shows; used while the
   * screen is built only.
   * @param motion Whether the ships and the beacons may move.
   */
  LobbyScreen(IScreenNavigator& navigator, IServerConnection& connection,
              std::unique_ptr<IInterfaceCanvas> canvas,
              PlayfieldMapping playfieldPointAt, const KeyBindings& keys,
              const KeyNaming& keyNaming, MotionPreference motion);

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

  void paintBays(std::span<const network::LobbyPlayer> players,
                 std::uint8_t ownNumber);

  IScreenNavigator* navigator_{};
  IServerConnection* connection_{};
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  sf::Keyboard::Scancode readyKey_{};
  MotionPreference motion_{};
  engine::Duration elapsed_{};
  bool readyKeyHeld_ = false;
  bool gameAsked_ = false;
  Menu menu_;
};

}  // namespace rtype::client
