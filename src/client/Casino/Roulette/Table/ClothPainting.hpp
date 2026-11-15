#pragma once

#include <cstdint>
#include <vector>
#include "ClothPlace.hpp"
#include "IInterfaceCanvas.hpp"
#include "RouletteBets.hpp"

namespace rtype::client {

/** @brief Paints a stack of chips on the spot of each of @p bets: a chip per
 * value its amount is made of, the largest at the bottom, CLOTH_STACK_CHIPS
 * at most. */
void paintClothBets(IInterfaceCanvas& canvas,
                    const std::vector<network::RouletteBet>& bets);

/** @brief Paints the cursor on @p place: corners in cyan around what it
 * frames (areaOf()). */
void paintClothCursor(IInterfaceCanvas& canvas, ClothPlace place);

/** @brief Lights @p number on the cloth: its digits in gold, and gold corners
 * around its box. */
void paintDrawnNumber(IInterfaceCanvas& canvas, std::uint8_t number);

}  // namespace rtype::client
