#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include "NetworkConstants.hpp"
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
constexpr std::size_t FOURTH_PLAYER = 3;

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

/**
 * @brief Actions a test player holds, one bit each where the Inputs section of
 * the protocol page puts them: bit 0 up, bit 1 down, bit 2 left, bit 3 right,
 * bit 4 shoot.
 */
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
