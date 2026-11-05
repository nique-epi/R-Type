#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The answer of a server to a ServerInformationRequest: what it is,
 * how full it is, and the echo of the request, so the asker measures the
 * round trip without a clock shared with the server.
 *
 * The asker reads the round trip as the time since the request left, less
 * heldFor, both on its own clock: the time the request waited in the server
 * for its tick is left out, as the echo of a Heartbeat leaves it out.
 */
struct ServerInformation {
  static constexpr MessageType TYPE = MessageType::ServerInformation;

  /** @brief The sentAt of the request answered, unchanged. */
  std::uint32_t sentAt{};
  /** @brief Microseconds from the arrival of the request to the sending of
   * this answer. */
  std::uint32_t heldFor{};
  /** @brief The version of the protocol the server speaks, PROTOCOL_VERSION. */
  std::uint8_t protocolVersion{};
  /** @brief The version of the game it runs, 1 to MAXIMUM_GAME_VERSION_SIZE
   * bytes of printable UTF-8. */
  std::string gameVersion;
  /** @brief The name players read, 1 to MAXIMUM_SERVER_NAME_SIZE bytes of
   * printable UTF-8. */
  std::string name;
  /** @brief The players seated in its games, away ones included. */
  std::uint16_t playerCount{};
  /** @brief The games it hosts, whatever their state. */
  std::uint16_t gameCount{};
  /** @brief The seats a new player may take now: the empty seats of the games
   * that wait for players. */
  std::uint16_t freeSeatCount{};

  bool operator==(const ServerInformation&) const = default;
};

/**
 * @brief Writes the fields in the order they are declared.
 * @throws What decodePayload() throws for a value out of range; nothing is
 * written then.
 */
void encodePayload(ByteWriter& writer, const ServerInformation& message);

/**
 * @brief Reads a ServerInformation from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidServerTextException naming GAME_VERSION_FIELD or
 * SERVER_NAME_FIELD when that text breaks its rule.
 * @throws InvalidFieldValueException naming PLAYER_COUNT_FIELD when more
 * players are seated than the games hold, or FREE_SEAT_COUNT_FIELD when the
 * players and the free seats together are more than the games hold.
 * @throws TrailingBytesException when bytes follow freeSeatCount.
 */
[[nodiscard]] ServerInformation decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ServerInformation> type);

}  // namespace rtype::network
