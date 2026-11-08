#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <string_view>
#include "ConnectionScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ServerChoice.hpp"

/** @brief A connection screen opened as the server map opens it, after the
 * map wrote MAP_ADDRESS and MAP_PORT as the server of the last attempt, over a
 * recording connection and canvas. */
class ConnectionToMapServerFixture : public ::testing::Test {
 protected:
  ConnectionToMapServerFixture();

  /** @brief Writes a new game played hard with five lives and friendly fire
   * on as the game of the last attempt, as the new game dialog does on
   * CREATE, then opens the screen again. */
  void chooseANewGame();

  /** @brief Erases the nickname the field starts with, then types
   * @p nickname. */
  void typeNickname(std::u32string_view nickname);

  /** @brief Presses and releases Enter, then lets the press last. */
  void chooseEnter();

  [[nodiscard]] const RecordingServerConnection& connection() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

 private:
  /** @brief Builds the screen from the choice, in place of the one shown. */
  void open();

  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::ServerChoice choice_;
  std::unique_ptr<rtype::client::ConnectionScreen> screen_;
};
