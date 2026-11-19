#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "MapBallot.hpp"
#include "RecordingSessionOutbox.hpp"
#include "RecordingWaitingRoom.hpp"

namespace rtype::client::testing {

/** @brief A ballot over a recording waiting room, sending to a recording
 * outbox. */
class MapBallotFixture : public ::testing::Test {
 protected:
  /** @brief Opens the session of BALLOT_PLAYER_NUMBER. */
  void open();

  [[nodiscard]] MapBallot& ballot();
  [[nodiscard]] RecordingWaitingRoom& room();

  /** @returns The map of every MapVote sent, oldest first. */
  [[nodiscard]] std::vector<std::uint8_t> sentVotes() const;

 private:
  RecordingWaitingRoom room_;
  RecordingSessionOutbox outbox_;
  MapBallot ballot_{room_};
};

}  // namespace rtype::client::testing
