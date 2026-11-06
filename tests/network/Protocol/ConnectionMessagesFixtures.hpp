#pragma once

#include "ByteWriter.hpp"
#include "ConnectionRequest.hpp"

namespace rtype::network::testing {

/** @returns The smallest request the protocol allows: cookie 0 and a nickname
 * of one byte. */
ConnectionRequest smallestRequest();

/** @returns A request with SOME_COOKIE and ONE_BYTE_NICKNAME that asks for
 * @p game. */
ConnectionRequest requestFor(const GameChoice& game);

/** @brief Writes the end of a request that asks for any game: the kind 1 and
 * the number 0. */
void writeAnyGameChoice(ByteWriter& writer);

/** @brief Writes a request with SOME_COOKIE and ONE_BYTE_NICKNAME whose game
 * is @p game, unchecked, as a sender may forge it. */
void writeRequestFor(ByteWriter& writer, const GameChoice& game);

}  // namespace rtype::network::testing
