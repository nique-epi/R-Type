#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The newest Heartbeat a side received from its peer, sent back in its
 * own Heartbeat: the peer takes the time it was held away from the time since
 * it was sent, which leaves the round trip.
 */
struct HeartbeatEcho {
  /** @brief The sentAt of the Heartbeat echoed, unchanged. */
  std::uint32_t sentAt{};
  /** @brief Microseconds from the arrival of that Heartbeat to the sending of
   * the one that echoes it. */
  std::uint32_t heldFor{};

  bool operator==(const HeartbeatEcho&) const = default;
};

/**
 * @brief Sent by each side of a session every HEARTBEAT_INTERVAL, whatever
 * else it sends, so its peer knows it is still there and both measure the
 * round trip between them (see RoundTripMeter). The server also tells in it
 * which tick it is at, so the client knows which tick of the server falls on
 * which instant of its own clock (see ServerClock).
 */
struct Heartbeat {
  static constexpr MessageType TYPE = MessageType::Heartbeat;

  /** @brief When it was sent, in microseconds on the clock of its sender,
   * modulo 2^32: only its difference with a later time of the same clock means
   * something. */
  std::uint32_t sentAt{};
  /** @brief The newest Heartbeat the sender received in the session; empty
   * before the first one. */
  std::optional<HeartbeatEcho> echo;
  /** @brief The tick the sender was at when it sent it: only the server
   * says it, and only while a game runs; empty before and in every Heartbeat
   * of a client. */
  std::optional<std::uint32_t> tick = std::nullopt;

  bool operator==(const Heartbeat&) const = default;
};

/**
 * @brief Writes sentAt, then whether an echo follows, then the sentAt and the
 * heldFor of the echo, both 0 without one, then whether a tick follows, then
 * the tick, 0 without one, so every Heartbeat has the same size.
 */
void encodePayload(ByteWriter& writer, const Heartbeat& message);

/**
 * @brief Reads a Heartbeat from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidBoolException when the byte that says whether an echo follows
 * is neither FALSE_WIRE_VALUE nor TRUE_WIRE_VALUE, nor the byte that says
 * whether a tick follows.
 * @throws TrailingBytesException when bytes follow the tick.
 * @throws InvalidFieldValueException naming ECHO_SENT_AT_FIELD,
 * ECHO_HELD_FOR_FIELD or TICK_FIELD when a Heartbeat without an echo, or
 * without a tick, carries a value other than 0 there.
 */
[[nodiscard]] Heartbeat decodePayload(std::span<const std::byte> payload,
                                      std::type_identity<Heartbeat> type);

}  // namespace rtype::network
