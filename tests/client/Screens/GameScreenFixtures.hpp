#pragma once

#include <cstdint>
#include "GameEnded.hpp"
#include "WorldState.hpp"

class RecordingServerConnection;

/** @brief Accepts a connection as player 2, NOVA, of a waiting room where ACE
 * is player 1 and KAI player 3, none ready. */
void seatThreePilots(RecordingServerConnection& connection);

/** @returns A world state of that tick listing the first pilot with its
 * score and lives, and no entity. */
rtype::network::WorldState stateScoringTheFirstPilot(std::uint32_t tick);

/** @returns The end of a game of the three pilots of seatThreePilots(), with
 * the scores FIRST_PILOT_FINAL, SECOND_PILOT_FINAL and THIRD_PILOT_FINAL. */
[[nodiscard]] rtype::network::GameEnded endOfThreePilots();
