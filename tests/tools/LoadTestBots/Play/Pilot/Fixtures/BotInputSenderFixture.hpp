#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include "BotInputSender.hpp"
#include "BotPilot.hpp"
#include "BotPilotTestConstants.hpp"
#include "BotSight.hpp"
#include "EntityMessage.hpp"
#include "FixedEntityTypeTable.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "RecordingWaitingRoom.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

/** @brief A sender drawing from PILOT_SEED, over a waiting room whose start
 * the test sets and a link that writes down each input. */
class BotInputSenderFixture : public ::testing::Test {
 protected:
  /** @brief Starts, or ends, the game of the waiting room. */
  void setGameStarted(bool started);

  /** @returns In how many of @p frames frames of @p frame the sender said an
   * input went out. */
  [[nodiscard]] std::size_t advanceFor(
      std::size_t frames,
      engine::Duration frame = engine::SIMULATION_TICK_DURATION);

  [[nodiscard]] const RecordingPlayerInputLink& link() const;

  /** @brief Hands @p message to the sender for the bot of @p playerNumber
   * (see BotInputSender::confirm()). */
  void confirmFrom(const client::EntityMessage& message,
                   std::uint8_t playerNumber);

  /** @returns A world state listing the player @p playerNumber, whose input
   * ACKNOWLEDGED_INPUT the server applied, and its ship SHIP_ENTITY_ID. */
  [[nodiscard]] static client::EntityMessage stateAcknowledgingFor(
      std::uint8_t playerNumber);

 private:
  RecordingWaitingRoom room_;
  RecordingPlayerInputLink link_;
  FixedEntityTypeTable types_;
  BotSight sight_{types_};
  BotInputSender sender_{std::make_unique<BotPilot>(PILOT_SEED), room_, link_};
};

}  // namespace rtype::tools::testing
