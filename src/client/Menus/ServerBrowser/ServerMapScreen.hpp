#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <vector>
#include "AmbienceScope.hpp"
#include "ColoredPixel.hpp"
#include "GamePanel.hpp"
#include "IGameCatalog.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerCatalog.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlanetPose.hpp"
#include "PlayfieldMapping.hpp"
#include "ServerChoice.hpp"
#include "ServerMapChoice.hpp"
#include "ServerMapFrame.hpp"
#include "ServerMapScreenParts.hpp"
#include "SessionEndWatch.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The map of the servers PLAY opens over the title, and RETRY under the
 * connection screen of a lost game, as the mockup C1
 * v13 draws it: the planet of the player cut by the left edge, each server the
 * master lists standing past it by its ping, the chosen one framed with its
 * bubble.
 *
 * The arrows choose the star they point at, Tab the next server by ping.
 * MENU_CONFIRM_KEY, or the wheel up, zooms on the chosen server; once zoomed,
 * MENU_CONFIRM_KEY opens the panel of its games, read from the master, and on
 * a waiting game joins it, on + NEW GAME pushes NEW_GAME_SCREEN over the map.
 * On the panel, the pointer chooses the row it lies over, and a click on a
 * row acts on it as MENU_CONFIRM_KEY does.
 * Joining puts the server and the game in the ServerChoice and pushes
 * CONNECTION_TO_MAP_SERVER_SCREEN, which asks only for the nickname, then
 * connects and opens the waiting room: a game of the panel is asked for by its
 * number, with the rules the master lists for it. The chips: QUICK PLAY joins
 * any game of the server with the lowest ping among those of this version
 * that list fewer players than their games hold, and is disabled when none
 * does; RANKING opens the leaderboard, ADDRESS the connection form, BACK goes
 * one stage back, then closes the map. A place of the whole map
 * (MAP_PLACES: the hangar, the shop), or its key, pushes its screen.
 *
 * The list is read when the map opens, again every
 * SERVER_LIST_REFRESH_INTERVAL, on REFRESH_KEY, and GAMES_RECHECK_DELAY after
 * the player came back from a server (SessionEndWatch). A key that changes the
 * map acts once per hold.
 */
class ServerMapScreen : public IScreen {
 public:
  /** @param canvas Picture the map is painted on, never null. */
  ServerMapScreen(ServerMapScreenParts parts,
                  std::unique_ptr<IInterfaceCanvas> canvas);
  /** @brief The actions of the chips point at the screen, so it never
   * moves. */
  ServerMapScreen(const ServerMapScreen&) = delete;
  ServerMapScreen& operator=(const ServerMapScreen&) = delete;
  ServerMapScreen(ServerMapScreen&&) = delete;
  ServerMapScreen& operator=(ServerMapScreen&&) = delete;
  ~ServerMapScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Moves the choice with an arrow or Tab; confirms or refreshes once
   * per hold. */
  void keyPressed(const sf::Event::KeyPressed& key);
  /** @brief Goes one stage deeper, joins the chosen waiting game, or opens
   * the new game dialog on + NEW GAME. */
  void confirm();
  /** @brief Acts on a click at @p pixel of the interface: on the panel of the
   * games, chooses the row under it and acts on it as MENU_CONFIRM_KEY does;
   * elsewhere, opens the hangar from its star, or the games of the server
   * whose star it is. */
  void pointerPressed(sf::Vector2i pixel);
  /** @brief Goes one stage back, or closes the map from the whole map. */
  void stepBack();
  /** @brief Reads the list again, and the games of the chosen server when
   * their panel is open. */
  void refresh();
  void repaint();

  IScreenNavigator* navigator_;
  AmbienceScope ambience_;
  IServerCatalog* servers_;
  IGameCatalog* games_;
  ServerChoice* choice_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  /** @brief Where the map lays its parts at the text size of the canvas, and
   * how far the picture is scrolled down, 0 to lastMapScrollOf(frame_). */
  ServerMapFrame frame_;
  int scroll_{};
  MotionPreference motion_;
  ServerMapChoice map_;
  GamePanel panel_;
  SessionEndWatch sessionEnd_;
  KeyChip quickPlayChip_;
  KeyChip rankingChip_;
  KeyChip addressChip_;
  KeyChip backChip_;
  engine::Duration elapsed_{};
  engine::Duration sinceRefresh_{};
  std::optional<sf::Keyboard::Key> heldKey_;
  /** @brief The planet last painted and its pixels: the planet turns at
   * PLANET_FRAMES_PER_SECOND, slower than the frames, and not at all under
   * reduced motion, so its pixels are computed again only when it moved. */
  std::optional<PlanetPose> planetPose_;
  std::vector<ColoredPixel> planetPixels_;
  /** @brief Whether the whole map shows the corners and the bubble of the
   * chosen server: the pointer is over it, or an arrow or Tab chose it. */
  bool chosenShown_{false};
  /** @brief The place of MAP_PLACES the pointer is over, if any: its corners
   * and its bubble show, and the chosen server shows none. One at most, so
   * two places never show their bubbles together. */
  std::optional<std::size_t> placeShown_;
};

}  // namespace rtype::client
