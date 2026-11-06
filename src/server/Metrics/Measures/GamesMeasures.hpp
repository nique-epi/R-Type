#pragma once

#include <vector>
#include "GameInstance.hpp"
#include "GamesCensus.hpp"
#include "IClock.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "SystemTime.hpp"

namespace rtype::server {

/** @returns What @p game holds now, the game of a game process. Use it from
 * the simulation thread only. */
[[nodiscard]] GamesCensus censusOf(GameInstance& game);

/**
 * @brief Has the systems of @p game read @p clock (see
 * SystemScheduler::measureWith()); nullptr stops them.
 * @param clock Must stay valid until the next call.
 */
void measureSystemsOf(GameInstance& game, const engine::IClock* clock);

/**
 * @returns How long each system of @p game ran since the previous call, added
 * up by name, each name once, in the order first met.
 */
[[nodiscard]] std::vector<engine::SystemTime> takeSystemTimesOf(
    GameInstance& game);

/**
 * @returns The round trip of every player of @p game measured, in order of
 * number, each under NUMBER_OF_THE_GAME; a player measured nothing with yet is
 * left out. Use it from the simulation thread only.
 */
[[nodiscard]] std::vector<PlayerRoundTripMeasure> roundTripsOf(
    GameInstance& game);

}  // namespace rtype::server
