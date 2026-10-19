#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The screen PLAY opens over the title, which it hides: the address, the
 * port and the nickname to join a server with, CONNECT and BACK, then a status
 * line that tells how the attempt stands.
 *
 * Building it drops any earlier attempt, so it always starts from no attempt,
 * and its fields start with the values given at launch. MENU_CONFIRM_KEY on a
 * field, or CONNECT, checks the fields: an address that is not IPv4, a port
 * outside LOWEST_PORT to HIGHEST_PORT or an empty nickname is named on the
 * status line, and the attempt in progress is dropped without a new one.
 * Otherwise a new attempt replaces it. At each update the status line shows the
 * status of the connection; once the server has accepted the player for
 * ACCEPTED_STATUS_DURATION, the screen opens the waiting room over itself,
 * once per attempt. BACK, or MENU_CANCEL_KEY, drops the attempt and closes
 * the screen.
 */
class ConnectionScreen : public IScreen {
 public:
  /**
   * @param navigator Screens the connection screen lives among, which must
   * outlive it.
   * @param connection Connection the attempts go through, which must outlive
   * the screen.
   * @param canvas Picture the menu is painted on, never null.
   * @param playfieldPointAt Gives the point of the playfield under a pixel of
   * the window.
   * @param server Address and port the fields start with.
   * @param nickname Nickname the field starts with.
   */
  ConnectionScreen(IScreenNavigator& navigator, IServerConnection& connection,
                   std::unique_ptr<IInterfaceCanvas> canvas,
                   PlayfieldMapping playfieldPointAt,
                   const network::Endpoint& server, std::string_view nickname);

  /** @brief The actions of the menu point at the screen, so it never moves. */
  ConnectionScreen(const ConnectionScreen&) = delete;
  ConnectionScreen& operator=(const ConnectionScreen&) = delete;
  ConnectionScreen(ConnectionScreen&&) = delete;
  ConnectionScreen& operator=(ConnectionScreen&&) = delete;
  ~ConnectionScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  [[nodiscard]] Menu buildMenu(const network::Endpoint& server,
                               std::string_view nickname);

  /** @brief Starts an attempt with the values of the fields, or names the field
   * to fix. */
  void connect();

  /** @brief Drops the attempt, then asks for the screen to be closed. */
  void leave();

  /** @brief Opens the waiting room once the server has accepted the player
   * for ACCEPTED_STATUS_DURATION. */
  void followConnection(const ConnectionStatus& status,
                        engine::Duration elapsed);

  /** @returns The line naming the field to fix, or telling how the attempt
   * stands; empty when there is no attempt. */
  [[nodiscard]] std::string statusLine(const ConnectionStatus& status) const;

  /** @brief Paints the menu, its status line telling how the attempt stands,
   * read once per update so the line and the opening of the waiting room
   * agree. */
  void repaint(const ConnectionStatus& status);

  IScreenNavigator* navigator_{};
  IServerConnection* connection_{};
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  std::string_view fieldProblem_;
  engine::Duration acceptedFor_{};
  bool lobbyAsked_ = false;
  Menu menu_;
};

}  // namespace rtype::client
