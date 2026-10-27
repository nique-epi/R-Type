#pragma once

#include <cstdint>
#include <optional>
#include "LoopbackPeer.hpp"

namespace rtype::tools {

/**
 * @brief Joins the server listening on @p serverPort of the loopback address
 * as a player, through @p peer: a ConnectionRequest, the ConnectionChallenge,
 * the request again with its cookie, then the ConnectionAccepted.
 * @returns The session token of the acceptance, or nothing when an answer is
 * another message or does not come within ANSWER_TIMEOUT.
 */
[[nodiscard]] std::optional<std::uint64_t> joinServer(LoopbackPeer& peer,
                                                      std::uint16_t serverPort);

/**
 * @returns Whether the server listening on @p serverPort answers a
 * ConnectionRequest within ANSWER_TIMEOUT, whatever the answer. The request
 * goes from a new socket, so it comes from a sender the server does not know.
 */
[[nodiscard]] bool serverAnswers(std::uint16_t serverPort);

/**
 * @brief Waits on @p peer, for CLIENT_WAIT_TIMEOUT at most, for a client to
 * send a ConnectionRequest, then accepts it as player FIRST_PLAYER_NUMBER
 * with TOOL_SESSION_TOKEN, in a datagram of sequence SEQUENCE_BEFORE_THE_FLOOD.
 * @returns The port of that client, or nothing when none asked.
 */
[[nodiscard]] std::optional<std::uint16_t> acceptClient(LoopbackPeer& peer);

/**
 * @returns Whether a datagram from @p clientPort whose header carries
 * TOOL_SESSION_TOKEN reaches @p peer within CLIENT_SIGN_OF_LIFE_TIMEOUT: the
 * client is still in the session acceptClient() opened.
 */
[[nodiscard]] bool clientStillInSession(LoopbackPeer& peer,
                                        std::uint16_t clientPort);

}  // namespace rtype::tools
