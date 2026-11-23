#pragma once

#include <gtest/gtest.h>
#include "AnnouncedStart.hpp"
#include "GameLoadedReminder.hpp"
#include "GameStarted.hpp"
#include "MessageDispatcher.hpp"
#include "RecordingSessionOutbox.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client::testing {

/** @brief A waiting room and the start it announces, fed through the
 * dispatcher the room registered its handlers on, as the session feeds it. */
class AnnouncedStartFixture : public ::testing::Test {
 protected:
  /** @brief Opens the session of STARTING_PLAYER_NUMBER. */
  void open();

  /** @brief Hands @p started to the dispatcher, as a datagram of the session
   * would. */
  void deliver(const network::GameStarted& started);

  [[nodiscard]] WaitingRoom& room();
  [[nodiscard]] const AnnouncedStart& start() const;

 private:
  network::MessageDispatcher dispatcher_;
  GameLoadedReminder reminder_;
  AnnouncedStart start_;
  WaitingRoom room_{dispatcher_, reminder_, start_};
  RecordingSessionOutbox outbox_;
};

/** @returns The GameStarted of a level opening in ANNOUNCED_BIOME. */
[[nodiscard]] network::GameStarted startInTheCaves();

}  // namespace rtype::client::testing
