#pragma once

#include <chrono>
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
 * @brief Launch option followed by the seed the random draws of the games start
 * from, a whole number from 0 to 4294967295.
 */
constexpr std::string_view SEED_OPTION = "--seed";

/**
 * @brief Module name the server writes its log lines under.
 */
constexpr std::string_view SERVER_LOGGER_NAME = "Server";

/**
 * @brief Why the DatagramSorter drops a datagram, as its lines say; a header
 * it does not speak is dropped for FOREIGN_HEADER_DROP_REASON.
 */
constexpr std::string_view SHORTER_THAN_HEADER_DROP_REASON =
    "it is shorter than the packet header";
constexpr std::string_view MESSAGE_SIZES_DROP_REASON =
    "the sizes of its messages do not add up to its end";
constexpr std::string_view STRANGER_DROP_REASON =
    "its sender was not admitted, and it holds no connection request";
constexpr std::string_view WRONG_TOKEN_DROP_REASON =
    "it carries another session token than the player of that address";

/**
 * @brief Where the server reads the waves of each difficulty, from the folder
 * it is started in: the root of the repository, where the build puts it.
 */
constexpr std::string_view WAVE_FILE_PATH = "assets/waves/waves.toml";

/**
 * @brief The difficulty every game is played at, until the rules of a game can
 * choose another one.
 */
constexpr std::string_view DEFAULT_DIFFICULTY = "normal";

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
 * Provisional: while the simulation keeps its pace, a client sends at most two
 * requests in one tick (the one sent again every 500 ms and the answer to a
 * challenge), so 16 is eight clients joining at once, about 9 microseconds of
 * answers per tick. After a stall, a client answers every challenge sent for
 * its waiting requests, up to 10 in one tick before it gives up at 5 s. Above
 * the room, the oldest are discarded and the clients send again; a limit per
 * address does not exist yet.
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
 * @brief The longest time between two LobbyState sent to the players while
 * the game waits for them. Provisional, see the protocol page.
 */
constexpr engine::Duration LOBBY_STATE_INTERVAL =
    std::chrono::milliseconds(500);

/**
 * @brief Simulation ticks between two LobbyState that show the same room:
 * LOBBY_STATE_INTERVAL in whole ticks, rounded down, so two of them are never
 * further apart than the interval. At 60 ticks per second it is 30 ticks,
 * 499.99998 ms; adding up tick durations until they reach the interval would
 * take 31 ticks, 516.7 ms.
 */
constexpr std::size_t TICKS_PER_LOBBY_STATE =
    engine::wholeTicksIn(LOBBY_STATE_INTERVAL);

static_assert(TICKS_PER_LOBBY_STATE >= 1,
              "LOBBY_STATE_INTERVAL must last at least one simulation tick");

/**
 * @brief The time between two GameEnded sent to the players while their game
 * is finished. Provisional, see the protocol page.
 */
constexpr engine::Duration GAME_ENDED_INTERVAL = std::chrono::milliseconds(500);

/**
 * @brief Simulation ticks between two GameEnded: GAME_ENDED_INTERVAL in whole
 * ticks, rounded down, so two of them are never further apart than the
 * interval. At 60 ticks per second it is 30 ticks.
 */
constexpr std::size_t TICKS_PER_GAME_ENDED =
    engine::wholeTicksIn(GAME_ENDED_INTERVAL);

static_assert(TICKS_PER_GAME_ENDED >= 1,
              "GAME_ENDED_INTERVAL must last at least one simulation tick");

/**
 * @brief Timer resolution, in milliseconds, the server asks Windows for while
 * the simulation runs, so a sleeping thread wakes close to its deadline.
 */
constexpr std::uint32_t FINE_TIMER_RESOLUTION_MILLISECONDS = 1;

}  // namespace rtype::server
