#pragma once

#include <cstdint>
#include "RouletteBets.hpp"

namespace rtype::server {

/**
 * @returns Whether @p bet lies on the cloth of a single zero roulette, read
 * across: 0 at the head, then twelve streets of three (1 2 3, 4 5 6, ...). A
 * split, a corner or a street must not cross the end of a street or of the
 * cloth, a dozen starts at 1, 13 or 25, a column at 1, 2 or 3, and an even
 * chance names no number (its first number is 0). A bet that touches the zero
 * other than a straight on it is not placeable.
 */
[[nodiscard]] bool isPlaceableBet(const network::RouletteBet& bet);

/**
 * @returns The chips @p bet returns when @p drawn comes out: its amount times
 * the return of its kind (CasinoConstants.hpp) when it covers @p drawn, its
 * stake included; 0 when it does not, and for a bet that is not placeable. The
 * zero loses every even chance.
 */
[[nodiscard]] std::uint64_t payoutOf(const network::RouletteBet& bet,
                                     std::uint8_t drawn);

}  // namespace rtype::server
