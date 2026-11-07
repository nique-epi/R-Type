#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <optional>
#include <vector>
#include "ColoredPixel.hpp"
#include "GamePanel.hpp"
#include "IGameCatalog.hpp"
#include "IInterfaceCanvas.hpp"
#include "IMasterAddress.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerCatalog.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlanetPose.hpp"
#include "PlayfieldMapping.hpp"
#include "ServerChoice.hpp"
#include "ServerMapChoice.hpp"
#include "ServerMapScreenParts.hpp"
#include "ServerRow.hpp"
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
 * a waiting game joins it. Joining puts the server in the ServerChoice and
 * pushes CONNECTION_TO_CHOSEN_SERVER_SCREEN, which connects at once and opens
 * the waiting room; the server places the player in its first waiting game,
 * since the protocol names no game yet. The chips: QUICK PLAY joins the
 * server of this version with the lowest ping, RANKING opens the leaderboard,
 * ADDRESS the connection form, BACK goes one stage back, then closes the map.
 * The hangar star of the whole map, or HANGAR_KEY, pushes HANGAR_SCREEN.
 * MASTER, on the right of the title, opens the form of the master the map
 * reads, which is written under it.
 *
 * The list is read when the map opens, again every
 * SERVER_LIST_REFRESH_INTERVAL and on REFRESH_KEY. A key that changes the map
 * acts once per hold.
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
  /** @brief Goes one stage deeper, or joins the chosen waiting game. */
  void confirm();
  /** @brief Goes one stage back, or closes the map from the whole map. */
  void stepBack();
  /** @brief Reads the list again, and the games of the chosen server when
   * their panel is open. */
  void refresh();
  /** @brief Puts @p server in the ServerChoice and opens the connection to
   * it. */
  void join(const ServerRow& server);
  void repaint();

  IScreenNavigator* navigator_;
  IServerCatalog* servers_;
  IGameCatalog* games_;
  ServerChoice* choice_;
  const IMasterAddress* master_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  MotionPreference motion_;
  ServerMapChoice map_;
  GamePanel panel_;
  KeyChip quickPlayChip_;
  KeyChip rankingChip_;
  KeyChip addressChip_;
  KeyChip masterChip_;
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
  /** @brief Whether the pointer is over the hangar star: its corners and its
   * bubble show, and the chosen server shows none. */
  bool hangarShown_{false};
};

}  // namespace rtype::client
