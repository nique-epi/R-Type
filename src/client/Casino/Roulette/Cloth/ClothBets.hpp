#pragma once

#include "ClothPlace.hpp"
#include "RouletteBets.hpp"

namespace rtype::client {

/**
 * @returns The bet that lies on @p place, its amount 0, as the server reads
 * the cloth: on a number, a straight; on the line between two numbers of a
 * street, a split; on the line between two streets, a split across; where
 * four numbers meet, a corner; on the edge under a street, a street; where
 * that edge meets the line between two streets, a six line; then the zero,
 * the columns, the dozens and the even chances. @p place is one of the
 * places placeAfter() reaches.
 */
[[nodiscard]] network::RouletteBet betAt(ClothPlace place);

/** @returns The place @p bet lies on, the one betAt() gives it back from; the
 * zero on its middle row. */
[[nodiscard]] ClothPlace placeOf(const network::RouletteBet& bet);

}  // namespace rtype::client
