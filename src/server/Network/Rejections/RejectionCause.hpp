#pragma once

#include <cstdint>

namespace rtype::server {

/**
 * @brief Why the network thread rejected a datagram, one value per count of the
 * rejection report. The values are fixed by the code, never by what a sender
 * writes, so no sender can make the server count anything else.
 */
enum class RejectionCause : std::uint8_t {
  /** @brief Shorter than the packet header. */
  ShorterThanHeader,
  /**
   * @brief A header of another protocol, of another version the server does
   * not answer, or with a flag this version does not know.
   */
  ForeignHeader,
  /** @brief Message sizes that do not add up to the end of the datagram. */
  MessageSizes,
  /** @brief From a sender that was not admitted, with no message sent outside
   * any session. */
  Stranger,
  /** @brief From the address of an admitted player, with another token. */
  WrongSessionToken,
  /**
   * @brief Over the rate limit of its sender, whether a player or an address
   * and port among the requests sent outside any session.
   */
  OverRateLimit,
  /** @brief The datagram that put its sender over the limit once too often,
   * so that it is banned from now on. */
  NewlyBanned,
  /** @brief Sent during the ban of its sender. */
  Banned,
  /** @brief Made the player queue discard its oldest datagram. */
  PlayerQueueFull,
  /** @brief Made the connection request queue discard its oldest datagram. */
  RequestQueueFull,
  /** @brief Made the information request queue discard its oldest datagram. */
  InformationQueueFull,
};

}  // namespace rtype::server
