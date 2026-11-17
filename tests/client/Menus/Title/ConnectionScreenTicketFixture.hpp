#pragma once

#include <gtest/gtest.h>
#include "ConnectionScreen.hpp"
#include "FakeAccountSession.hpp"
#include "FakeTicketLink.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ServerChoice.hpp"
#include "TimeConstants.hpp"

/** @brief A connection screen alone, the player signed in with the token
 * SIGNED_IN_TOKEN, and a ticket link the test answers by hand. */
class ConnectionScreenTicketFixture : public ::testing::Test {
 protected:
  ConnectionScreenTicketFixture();

  /** @brief Presses and releases Enter on the form, which starts an attempt,
   * then lets the press last. */
  void confirm();

  /** @brief Lets @p elapsed go by in one frame. */
  void wait(rtype::engine::Duration elapsed);

  [[nodiscard]] RecordingServerConnection& connection();
  [[nodiscard]] FakeTicketLink& tickets();
  [[nodiscard]] FakeAccountSession& account();
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

 private:
  FakeAccountSession account_;
  FakeTicketLink tickets_;
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::ServerChoice choice_;
  rtype::client::ConnectionScreen screen_;
};
