#pragma once

#include <gtest/gtest.h>
#include <memory>
#include "ConnectionScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ServerChoice.hpp"

/** @brief A connection screen that opens the waiting room, its choice of
 * server marked for the casino, as CASINO of the destination screen marks
 * it, over a recording connection and navigator. */
class ConnectionTowardsTheCasinoFixture : public ::testing::Test {
 protected:
  ConnectionTowardsTheCasinoFixture();

  /** @brief Has the server accept the player, then lets the acceptance show
   * long enough for the screen to follow its goal. */
  void accept();

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const rtype::client::ServerChoice& choice() const;

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::ServerChoice choice_;
  std::unique_ptr<rtype::client::ConnectionScreen> screen_;
};
