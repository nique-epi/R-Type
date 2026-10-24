#pragma once

#include <cstddef>
#include "Star.hpp"
#include "Starfield.hpp"
#include "TimeConstants.hpp"

/** @returns @p duration in seconds. */
float seconds(rtype::engine::Duration duration);

/** @brief Advances @p starfield by @p total, in frames of the longest step
 * the background takes at once. */
void advanceBy(rtype::client::Starfield& starfield,
               rtype::engine::Duration total);

/** @returns The speed of the stars of the layer at @p depth. */
float speedOf(rtype::client::StarDepth depth);

/** @returns How many stars of @p starfield are at @p depth. */
std::size_t countAt(const rtype::client::Starfield& starfield,
                    rtype::client::StarDepth depth);

/** @brief Expects every star of @p left at the column of the star of the same
 * index in @p right, within POSITION_TOLERANCE. */
void expectSameColumns(const rtype::client::Starfield& left,
                       const rtype::client::Starfield& right);

/** @returns How many pixels a starfield of SEED paints at that speed
 * factor. */
std::size_t plotCountAt(float speedFactor);
