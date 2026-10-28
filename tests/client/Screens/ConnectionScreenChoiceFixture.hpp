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

  /** @brief Presses and releases Enter on the screen shown, then lets the
   * press last. */
  void chooseEnter();

  [[nodiscard]] const RecordingServerConnection& connection() const;

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::ServerChoice choice_;
  std::unique_ptr<rtype::client::ConnectionScreen> screen_;
};
