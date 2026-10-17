#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Name of the server program, at the start of its usage and of the
 * errors it prints about its arguments.
 */
constexpr std::string_view PROGRAM_NAME = "r-type_server";

/**
 * @brief Module name the server writes its log lines under.
 */
constexpr std::string_view SERVER_LOGGER_NAME = "Server";

/**
 * @brief Module name the simulation thread writes its tick reports under.
 */
constexpr std::string_view SIMULATION_LOGGER_NAME = "Simulation";

/**
 * @brief Room of the queue of the datagrams of admitted players waiting for the
 * simulation thread.
 *
 * Provisional: about one second of the inputs of four players sending 60 per
 * second, far more than the simulation thread lets pile up between two ticks.
 */
constexpr std::size_t INCOMING_DATAGRAM_QUEUE_CAPACITY = 256;

/**
 * @brief Room of the queue of connection requests waiting for the simulation
 * thread, so the most requests a tick answers.
 *
 * Provisional: a client sends at most two requests in one tick (the one sent
 * again every 500 ms and the answer to a challenge), so 16 is eight clients
 * joining at once, about 9 microseconds of answers per tick. Above that, the
 * oldest are discarded and the clients send again; a limit per address does
 * not exist yet.
 */
constexpr std::size_t CONNECTION_REQUEST_QUEUE_CAPACITY = 16;

/**
 * @brief Room of the queue of datagrams the simulation thread leaves for the
 * network thread to send. Provisional, like the incoming one.
 */
constexpr std::size_t OUTGOING_DATAGRAM_QUEUE_CAPACITY = 256;

/**
 * @brief Seconds between two reports on the simulation: how late its ticks
 * started, and how many received datagrams were discarded.
 */
constexpr std::size_t REPORT_SECONDS = 10;

/**
 * @brief Ticks between two reports on the simulation.
 */
constexpr std::size_t TICKS_PER_REPORT =
    REPORT_SECONDS * engine::SIMULATION_TICKS_PER_SECOND;

/**
 * @brief States of the world the server sends per second. Provisional, see the
 * protocol page.
 */
constexpr std::size_t WORLD_STATES_PER_SECOND = 20;

static_assert(engine::SIMULATION_TICKS_PER_SECOND % WORLD_STATES_PER_SECOND ==
                  0,
              "the state rate must divide the tick rate: a state is sent every "
              "whole number of ticks");

/**
 * @brief Simulation ticks between two states of the world.
 */
constexpr std::size_t TICKS_PER_WORLD_STATE =
    engine::SIMULATION_TICKS_PER_SECOND / WORLD_STATES_PER_SECOND;

/**
 * @brief Timer resolution, in milliseconds, the server asks Windows for while
 * the simulation runs, so a sleeping thread wakes close to its deadline.
 */
constexpr std::uint32_t FINE_TIMER_RESOLUTION_MILLISECONDS = 1;

}  // namespace rtype::server
