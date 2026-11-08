#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include "GameRules.hpp"
#include "LobbyScreen.hpp"
#include "PlayerSettings.hpp"
#include "RecordingChatChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "RecordingWaitingRoom.hpp"
#include "ShipLook.hpp"

/** @brief A waiting room built for a game whose rules the client knows or
 * does not, the local player accepted as player 1, over a recording canvas.
 * LobbyScreenFixture holds more member functions than a class may, so these
 * tests have a fixture of their own. */
class LobbyRulesFixture : public ::testing::Test {
 protected:
  /** @brief Opens a waiting room for a game played by @p rules, or of rules
   * the client does not know, then lets a frame go by. */
  void open(std::optional<rtype::network::GameRules> rules);

  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  RecordingWaitingRoom waitingRoom_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::PlayerSettings settings_;
  RecordingChatChannel chat_;
  rtype::client::ShipLook look_{};
  std::unique_ptr<rtype::client::LobbyScreen> screen_;
};
