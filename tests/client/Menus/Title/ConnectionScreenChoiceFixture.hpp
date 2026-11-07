#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <string_view>
#include "ConnectionScreen.hpp"
#include "ConnectionStart.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ServerChoice.hpp"

/** @brief Connection screens built one after the other from the same choice
 * of server, started with the launch values, over a recording connection. */
class ConnectionScreenChoiceFixture : public ::testing::Test {
 protected:
  ConnectionScreenChoiceFixture();

  /** @brief Selects the nickname field, replaces its text with @p nickname,
   * then chooses Enter there. */
  void connectAs(std::u32string_view nickname);

  /** @brief Builds a new screen from the same choice, in place of the one
   * shown, as the client does after a lost game. */
  void reopen();
  /** @brief Opens a new screen that starts its attempt as soon as it is
   * built, as the server map opens it. */
  void reopenConnectingAtOnce();

  /** @brief Writes MAP_ADDRESS and MAP_PORT as the server of the last
   * attempt, as the server map does when the player joins a server. */
  void chooseOnTheMap();

  /** @brief Presses and releases Enter on the screen shown, then lets the
   * press last. */
  void chooseEnter();

  [[nodiscard]] const RecordingServerConnection& connection() const;

 private:
  /** @brief Builds a new screen from the same choice, started by @p start, in
   * place of the one shown. */
  void openWith(rtype::client::ConnectionStart start);

  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::ServerChoice choice_;
  std::unique_ptr<rtype::client::ConnectionScreen> screen_;
};
