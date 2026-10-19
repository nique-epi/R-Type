#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Room of the queue of received datagrams waiting for the frame loop.
 *
 * Provisional: nearly thirteen seconds of the 20 world states per second of
 * version 0 of the protocol, far more than arrives between two frames. It will
 * be revised with the rates of the protocol.
 */
constexpr std::size_t INCOMING_DATAGRAM_QUEUE_CAPACITY = 256;

/**
 * @brief Room of the queue of datagrams the frame loop leaves for the network
 * thread to send.
 *
 * Provisional: about one second of the 60 inputs per second the client sends,
 * far more than waits before the network thread sends it.
 */
constexpr std::size_t OUTGOING_DATAGRAM_QUEUE_CAPACITY = 64;

/**
 * @brief Time between two ConnectionRequest of an attempt while the server
 * has not answered. Provisional, see the protocol page.
 */
constexpr std::chrono::milliseconds CONNECTION_REQUEST_INTERVAL{500};

/**
 * @brief Time after the start of an attempt at which it gives up, when the
 * server has neither accepted nor refused it. Provisional, see the protocol
 * page.
 */
constexpr std::chrono::seconds CONNECTION_ATTEMPT_TIMEOUT{5};

/**
 * @brief PlayerInput the game sends per second, one per frame at most.
 * Provisional, see the protocol page.
 */
constexpr std::size_t PLAYER_INPUTS_PER_SECOND = 60;

/** @brief Time between two PlayerInput of the game. */
constexpr engine::Duration PLAYER_INPUT_INTERVAL =
    std::chrono::duration_cast<engine::Duration>(std::chrono::seconds(1)) /
    PLAYER_INPUTS_PER_SECOND;

/** @brief Module name the connection to a server writes its log lines under. */
constexpr std::string_view CONNECTION_LOGGER_NAME = "Connection";

}  // namespace rtype::client
