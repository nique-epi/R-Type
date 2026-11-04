#pragma once

#include <span>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WorldState.hpp"

namespace rtype::network {

/**
 * @brief Writes the number of @p players, then each of them, as every message
 * that lists the players of a game does.
 */
void writePlayers(ByteWriter& writer, std::span<const PlayerState> players);

/**
 * @brief Writes one entity: its id, its type, then its two coordinates.
 * @throws InvalidCoordinateException when a coordinate is NaN.
 */
void writeEntity(ByteWriter& writer, const EntityState& entity);

/**
 * @brief Reads what writePlayers() wrote.
 * @throws BufferUnderflowException when the buffer is cut short.
 * @throws TooManyPlayersException when more than MAXIMUM_PLAYERS are announced.
 * @throws InvalidFieldValueException when a motion holds a bit PlayerState
 * does not use.
 */
[[nodiscard]] std::vector<PlayerState> readPlayers(ByteReader& reader);

/**
 * @brief Reads the number of entities, then each of them. An entity of unknown
 * type is left out: the others are read.
 * @throws BufferUnderflowException when the buffer is cut short.
 * @throws InvalidFieldValueException when an entity has the id 0.
 */
[[nodiscard]] std::vector<EntityState> readEntities(ByteReader& reader);

}  // namespace rtype::network
