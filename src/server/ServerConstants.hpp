#pragma once

#include <array>
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
 * @brief How many games the server hosts at once, unless told otherwise. They
 * are all created when the server starts and reused: a finished game waits for
 * players again.
 */
constexpr std::size_t DEFAULT_HOSTED_GAME_COUNT = 8;

/**
 * @brief The gap between the seeds of two games started from the same seed.
 *
 * A game draws the seed of each world it builds as its own seed plus the count
 * of worlds built, so a gap of 65536 keeps the worlds of one game away from
 * those of the next for 65536 worlds.
 */
constexpr std::uint32_t GAME_SEED_STRIDE = 65536;

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
 * @brief Launch option followed by the number of world states the server
 * sends to each player per second, a whole number from
 * LOWEST_WORLD_STATES_PER_SECOND to HIGHEST_WORLD_STATES_PER_SECOND.
 */
constexpr std::string_view STATE_RATE_OPTION = "--state-rate";

/**
 * @brief States of the world the server sends per second when STATE_RATE_OPTION
 * is not given.
 */
constexpr std::size_t DEFAULT_WORLD_STATES_PER_SECOND = 20;

/**
 * @brief The fewest states per second STATE_RATE_OPTION accepts. A client
 * removes an entity that no state listed for UNLISTED_ENTITY_TIMEOUT. When a
 * state is cut into several datagrams, one state per second would remove a
 * live entity as soon as the part that lists it is lost; two per second need
 * two such parts lost in a row.
 */
constexpr std::size_t LOWEST_WORLD_STATES_PER_SECOND = 2;

/**
 * @brief The most states per second STATE_RATE_OPTION accepts: one per tick.
 */
constexpr std::size_t HIGHEST_WORLD_STATES_PER_SECOND =
    engine::SIMULATION_TICKS_PER_SECOND;

/**
 * @brief Launch option followed by the ticks the server holds each PlayerInput
 * before it applies it, a whole number from LOWEST_INPUT_DELAY_TICKS to
 * HIGHEST_INPUT_DELAY_TICKS. The same delay holds for every player.
 */
constexpr std::string_view INPUT_DELAY_OPTION = "--input-delay";

/**
 * @brief Ticks an input is held when INPUT_DELAY_OPTION is not given: 50 ms.
 */
constexpr std::size_t DEFAULT_INPUT_DELAY_TICKS = 3;

/**
 * @brief The fewest ticks INPUT_DELAY_OPTION accepts: 0 applies an input in the
 * tick that reads it.
 */
constexpr std::size_t LOWEST_INPUT_DELAY_TICKS = 0;

/**
 * @brief The most ticks INPUT_DELAY_OPTION accepts: half a second.
 */
constexpr std::size_t HIGHEST_INPUT_DELAY_TICKS =
    engine::SIMULATION_TICKS_PER_SECOND / 2;

static_assert(DEFAULT_INPUT_DELAY_TICKS >= LOWEST_INPUT_DELAY_TICKS &&
                  DEFAULT_INPUT_DELAY_TICKS <= HIGHEST_INPUT_DELAY_TICKS,
              "the default input delay must be one the option accepts");

static_assert(DEFAULT_WORLD_STATES_PER_SECOND >=
                      LOWEST_WORLD_STATES_PER_SECOND &&
                  DEFAULT_WORLD_STATES_PER_SECOND <=
                      HIGHEST_WORLD_STATES_PER_SECOND,
              "the default state rate must be one the option accepts");

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

/**
 * @brief How long the handler of the closing of a Windows console waits for the
 * server to finish stopping before it returns. Windows ends the process 5000 ms
 * after that event whatever the handler does (HandlerRoutine,
 * learn.microsoft.com); the margin leaves time to leave main().
 */
constexpr engine::Duration CONSOLE_CLOSE_WAIT = std::chrono::seconds(4);

/**
 * @brief The stages of the server's loop, in the order each tick runs them:
 * the datagrams received are read, the game is simulated, then what changed is
 * sent. A module adds its work to one of them during its install().
 */
constexpr std::string_view INPUT_STAGE = "inputs";
constexpr std::string_view SIMULATION_STAGE = "simulation";
constexpr std::string_view OUTPUT_STAGE = "outputs";
constexpr std::array<std::string_view, 3> SERVER_LOOP_STAGES{
    INPUT_STAGE, SIMULATION_STAGE, OUTPUT_STAGE};

}  // namespace rtype::server
