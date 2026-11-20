#pragma once

#include <cstddef>
#include <string>
#include "BetsReceipt.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "RouletteBets.hpp"
#include "RoundBets.hpp"

namespace rtype::network::testing {

/** @returns Ten chips on 17 and 25 on red. */
[[nodiscard]] RouletteBets twoBets();

/** @returns The rooms of the tests: the room of the tests, betting. */
[[nodiscard]] CasinoRoomListing oneRoomListing();

/** @returns The state of the room of the tests once its number is drawn,
 * with two numbers before it and ACE seated, paid. */
[[nodiscard]] CasinoRoomState drawnRoomState();

/** @returns The text of drawnRoomState() as a master that does not list the
 * bets of the players writes it: its player has no member bets. */
[[nodiscard]] std::string roomStateTextWithoutBets();

/** @returns The receipt of twoBets() on the round of the tests. */
[[nodiscard]] BetsReceipt twoBetsReceipt();

/** @returns twoBets() on the round of the tests, paid once drawn. */
[[nodiscard]] RoundBets paidRoundBets();

/** @returns A body listing @p count straight bets on 1. */
[[nodiscard]] std::string straightBetsBody(std::size_t count);

}  // namespace rtype::network::testing
