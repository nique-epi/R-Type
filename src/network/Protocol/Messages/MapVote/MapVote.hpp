#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief The map the player votes for in the waiting room: its number in the
 * maps of the newest LobbyState, from 1, or NO_MAP_VOTE to vote for none. The
 * client sends it when the player changes it, and again while LobbyState
 * shows another vote, so a lost one is corrected within 500 ms. The server
 * keeps it only while the game takes the choices of the waiting room, and
 * only for a number of one of its maps.
 */
struct MapVote {
  static constexpr MessageType TYPE = MessageType::MapVote;

  std::uint8_t map{NO_MAP_VOTE};

  bool operator==(const MapVote&) const = default;
};

/**
 * @brief Writes map as one byte.
 */
void encodePayload(ByteWriter& writer, const MapVote& message);

/**
 * @brief Reads a MapVote from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is empty.
 * @throws TrailingBytesException when bytes follow it.
 */
[[nodiscard]] MapVote decodePayload(std::span<const std::byte> payload,
                                    std::type_identity<MapVote> type);

}  // namespace rtype::network
