#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include "NetworkConstants.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

/**
 * @brief Players of the game tests, as indexes into FIVE_PLAYER_PORTS: the
 * player at index i joins as player number i + 1.
 */
constexpr std::size_t FIRST_PLAYER = 0;
constexpr std::size_t SECOND_PLAYER = 1;
constexpr std::size_t THIRD_PLAYER = 2;
constexpr std::size_t FOURTH_PLAYER = 3;

/**
 * @brief The 5 s the protocol page gives the server before it closes the
 * session of a player it hears nothing from, at 60 ticks per second.
 */
constexpr std::size_t PROTOCOL_SESSION_TIMEOUT_TICKS = 300;

/**
 * @brief The 15 s of the protocol page during which the server keeps the seat
 * of a player it took for gone, at 60 ticks per second, so the session closes
 * after PROTOCOL_SESSION_TIMEOUT_TICKS plus these.
 */
constexpr std::size_t PROTOCOL_RECONNECTION_GRACE_TICKS = 900;
constexpr std::size_t PROTOCOL_SESSION_CLOSING_TICKS = 1200;

/**
 * @brief The 1 s of the protocol page after which a side sends a Heartbeat
 * when it sent nothing else, at 60 ticks per second.
 */
constexpr std::size_t PROTOCOL_HEARTBEAT_TICKS = 60;

/**
 * @brief What the line about the second player timing out holds.
 */
constexpr std::string_view SECOND_PLAYER_TIMED_OUT_LINE_TEXT =
    "player 2 timed out";

/**
 * @brief What the line about the second player leaving holds.
 */
constexpr std::string_view SECOND_PLAYER_LEFT_LINE_TEXT = "player 2 left";

/**
 * @brief What the line about a game in progress that every player left holds.
 */
constexpr std::string_view GAME_LEFT_BY_EVERY_PLAYER_LINE_TEXT =
    "game finished: no player left";

/**
 * @brief Sequences of the datagrams a test player sends, in the order it sends
 * them.
 */
constexpr std::uint16_t FIRST_SEQUENCE = 1;
constexpr std::uint16_t SECOND_SEQUENCE = 2;
constexpr std::uint16_t THIRD_SEQUENCE = 3;
constexpr std::uint16_t FOURTH_SEQUENCE = 4;

/**
 * @brief A few ticks, for the tests that check nothing changes while they
 * run.
 */
constexpr std::size_t SOME_TICKS = 3;

/** @brief The seed the games of the tests start their random draws from. */
constexpr std::uint32_t SOME_SEED = 7;

/**
 * @brief How far in front of a ship the shooting tests put a Drifter, in
 * logical units: the three missiles it takes reach it within the first second,
 * while it would take about two seconds to touch the ship.
 */
constexpr float BYDO_DISTANCE_AHEAD = 600.0F;

/**
 * @brief Where a game test puts a Bydo whose missile must not reach a ship
 * within the test: on screen, far ahead of the ships, which start at x 192. Its
 * first missile starts at 1076 after one and a half seconds and flies 6 units
 * per tick.
 */
constexpr game::Position BYDO_FAR_AHEAD_OF_THE_SHIPS{.x = 1500.0F, .y = 300.0F};

/**
 * @brief How many ticks a game runs for its first wave to be gone before the
 * second comes: the three Bydos of the first wave appear 4, 6 and 8 seconds in
 * and take about 8.5 seconds to cross the playfield, so the last leaves about
 * 16.6 seconds in, and the second wave comes a pause of 4 seconds after that.
 * 18 seconds in, the world holds the ships only.
 */
constexpr std::size_t FIRST_WAVE_GONE_TICKS =
    engine::wholeTicksIn(std::chrono::seconds(18));

/**
 * @brief How many ticks a game runs for the first Bydo of its second wave to
 * be in the world, and not its second: they appear about 20.6 and 22.3 seconds
 * in, so 21.5 seconds in, the world holds the ships and that one Bydo.
 */
constexpr std::size_t SECOND_WAVE_BEGUN_TICKS =
    engine::wholeTicksIn(std::chrono::milliseconds(21500));

/**
 * @brief Actions a test player holds, one bit each where the Inputs section of
 * the protocol page puts them: bit 0 up, bit 1 down, bit 2 left, bit 3 right,
 * bit 4 shoot. A player who holds nothing sends 0.
 */
constexpr std::uint8_t HOLDS_NOTHING = 0x00;
constexpr std::uint8_t HOLDS_UP = 0x01;
constexpr std::uint8_t HOLDS_DOWN = 0x02;
constexpr std::uint8_t HOLDS_LEFT = 0x04;
constexpr std::uint8_t HOLDS_RIGHT = 0x08;
constexpr std::uint8_t HOLDS_SHOOT = 0x10;

/**
 * @brief An actions byte with bit 5 set, which no PlayerInput may hold.
 */
constexpr std::uint8_t UNLISTED_ACTION = 0x20;

/**
 * @brief The session tokens of FIRST_PLAYER and SECOND_PLAYER in the input
 * tests, one each, and a sender of the same address that is not a player.
 */
constexpr std::array PLAYER_SESSION_TOKENS{SESSION_TOKEN, OTHER_SESSION_TOKEN};
constexpr std::size_t STRANGER = network::MAXIMUM_PLAYERS;

/**
 * @brief Places in the order the datagrams were sent through the test
 * network, the first one sent being 0.
 */
constexpr std::size_t FIRST_SENT = 0;
constexpr std::size_t SECOND_SENT = 1;
constexpr std::size_t THIRD_SENT = 2;
constexpr std::size_t FOURTH_SENT = 3;

/**
 * @brief The highest number an input can carry: an input numbered so is newer
 * than any other.
 */
constexpr std::uint32_t LARGEST_INPUT_NUMBER =
    std::numeric_limits<std::uint32_t>::max();

/**
 * @brief The shuffled deliveries: how many inputs the player sends, how many
 * orders are drawn, and how many datagrams each tick receives.
 */
constexpr std::uint32_t SHUFFLED_INPUT_COUNT = 30;
constexpr std::uint32_t SHUFFLE_SEED_COUNT = 20;
constexpr std::size_t DATAGRAMS_PER_TICK = 4;

/**
 * @brief Ticks that run in REJECTION_LINE_INTERVAL, the shortest time between
 * two lines about the datagrams a game drops.
 */
constexpr std::size_t TICKS_PER_REJECTION_LINE =
    static_cast<std::size_t>(network::REJECTION_LINE_INTERVAL.count()) *
    engine::SIMULATION_TICKS_PER_SECOND;

static_assert(SHUFFLED_INPUT_COUNT <= network::PLAYER_ACTION_BITS + 1U,
              "each shuffled input holds actions no other input holds, so "
              "the actions kept tell which input was kept");

/**
 * @brief How far a coordinate read from a WorldState may be from the position
 * it was sent for: a coordinate travels in steps of an eighth of a unit.
 */
constexpr float WIRE_COORDINATE_TOLERANCE =
    1.0F / network::WIRE_COORDINATE_STEPS_PER_UNIT;

}  // namespace rtype::server::testing
