#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Module name the server writes its log lines under.
 */
constexpr std::string_view SERVER_LOGGER_NAME = "Server";

/**
 * @brief Module name the simulation thread writes its tick reports under.
 */
constexpr std::string_view SIMULATION_LOGGER_NAME = "Simulation";

/**
 * @brief Room of the queue of received datagrams waiting for the simulation
 * thread.
 *
 * Provisional: about one second of the inputs of four players sending 60 per
 * second, far more than the simulation thread lets pile up between two ticks.
 */
constexpr std::size_t INCOMING_DATAGRAM_QUEUE_CAPACITY = 256;

/**
 * @brief Room of the queue of datagrams the simulation thread leaves for the
 * network thread to send. Provisional, like the incoming one.
 */
constexpr std::size_t OUTGOING_DATAGRAM_QUEUE_CAPACITY = 256;

/**
 * @brief Seconds between two reports of how late the ticks started.
 */
constexpr std::size_t LATENESS_REPORT_SECONDS = 10;

/**
 * @brief Ticks between two reports of how late the ticks started.
 */
constexpr std::size_t TICKS_PER_LATENESS_REPORT =
    LATENESS_REPORT_SECONDS * engine::SIMULATION_TICKS_PER_SECOND;

/**
 * @brief Timer resolution, in milliseconds, the server asks Windows for while
 * the simulation runs, so a sleeping thread wakes close to its deadline.
 */
constexpr std::uint32_t FINE_TIMER_RESOLUTION_MILLISECONDS = 1;

}  // namespace rtype::server
