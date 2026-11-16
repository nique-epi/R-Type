#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "ConnectionGoal.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionStart.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "ServerChoice.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The screen ADDRESS opens over the server map, a server or QUICK PLAY
 * of the map over it, PLAY over the title for a client launched with `-h`,
 * RETRY over the map after a lost game, and JOIN A SERVER over the
 * leaderboard, which it hides: the address, the port and the
 * nickname to join a server with, CONNECT and BACK, then a status line that
 * tells how the attempt stands. Joining a server or a new game of the map, it
 * asks only for the nickname, under JOIN_SERVER_HEADING: the server is the
 * one of the last attempt, which the map wrote (ConnectionStart::
 * NicknameOnConfirm).
 *
 * Building it drops any earlier attempt, so it always starts from no attempt.
 * Its fields start with the nickname of the ServerChoice and, for
 * ConnectionStart::OnConfirm, the server the form remembers
 * (ServerChoice::server); otherwise with the server of the last attempt, or
 * that same server before any. With ConnectionStart::AtOnce it then starts an
 * attempt with them, as CONNECT would. An attempt writes its server as the
 * last one, and only an OnConfirm screen writes it as the server the form
 * remembers, so the server map never changes what ADDRESS offers.
 * MENU_CONFIRM_KEY on a field, or CONNECT, checks the fields: an address that
 * is not IPv4, a port outside LOWEST_PORT to HIGHEST_PORT or an empty nickname
 * is named on the status line, and the attempt in progress is dropped without a
 * new one. Otherwise a new attempt replaces it. At each update the status line
 * shows the status of the connection. A screen of the server map whose server
 * answers that it is full starts the attempt again, with the same nickname, on
 * the next server QUICK PLAY kept (ServerChoice::nextServers), until one takes
 * the player or none is left, and shows the refusal of the last one only. Once
 * the server has accepted the player
 * for ACCEPTED_STATUS_DURATION, the screen follows its ConnectionGoal, once per
 * attempt: it opens the waiting room over itself, or closes. BACK, or
 * MENU_CANCEL_KEY, drops the attempt and closes the screen.
 */
class ConnectionScreen : public IScreen {
 public:
  /** @param canvas Picture the menu is painted on, never null. */
  ConnectionScreen(ConnectionScreenParts parts,
                   std::unique_ptr<IInterfaceCanvas> canvas);

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
  /** @brief Builds the form, its fields filled with @p shown and the
   * nickname of the choice. */
  [[nodiscard]] Menu buildMenu(ISoundPlayer& sounds,
                               const network::Endpoint& shown);

  /** @brief Starts an attempt with the values of the fields, or names the field
   * to fix. */
  void connect();

  /** @brief Drops the attempt, then asks for the screen to be closed. */
  void leave();

  /** @brief Follows the goal once the server has accepted the player for
   * ACCEPTED_STATUS_DURATION. */
  void followConnection(const ConnectionStatus& status,
                        engine::Duration elapsed);

  /** @returns The line naming the field to fix, or telling how the attempt
   * stands; empty when there is no attempt, and once a session was lost or
   * closed. */
  [[nodiscard]] std::string statusLine(const ConnectionStatus& status) const;

  /** @brief Paints the menu, its status line telling how the attempt stands,
   * read once per update so the line and what the goal asks agree. */
  void repaint(const ConnectionStatus& status);

  IScreenNavigator* navigator_{};
  IServerConnection* connection_{};
  ServerChoice* choice_{};
  ConnectionGoal goal_;
  /** @brief When the attempt starts, which also decides whether the form
   * shows the ADDRESS and PORT fields and whether an attempt writes the server
   * the form remembers. */
  ConnectionStart start_{};
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  std::string_view fieldProblem_;
  engine::Duration acceptedFor_{};
  bool goalReached_ = false;
  Menu menu_;
};

}  // namespace rtype::client
