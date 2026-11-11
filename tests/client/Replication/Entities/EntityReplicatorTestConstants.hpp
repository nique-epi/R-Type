#pragma once

#include <cstdint>
#include "Position.hpp"

namespace rtype::client::testing {

/**
 * @brief The network ids of the entities the tests announce, one per type, an
 * id no message announces, and a second enemy.
 */
constexpr std::uint32_t SHIP_ID = 7;
constexpr std::uint32_t PLAYER_MISSILE_ID = 8;
constexpr std::uint32_t BYDO_ID = 9;
constexpr std::uint32_t ENEMY_MISSILE_ID = 10;
constexpr std::uint32_t UNANNOUNCED_ID = 11;
constexpr std::uint32_t SECOND_BYDO_ID = 12;
constexpr std::uint32_t LOOK_ITEM_ID = 13;

/**
 * @brief Where a message first puts an entity in the tests, then where a later
 * message puts it, in logical units.
 */
constexpr game::Position FIRST_POSITION{.x = 100.0F, .y = 200.0F};

/** @brief A look a WorldState names for an entity, and the look of a state
 * older than it. */
constexpr std::uint8_t NAMED_LOOK = 3;
constexpr std::uint8_t OLDER_LOOK = 1;
constexpr game::Position SECOND_POSITION{.x = 150.5F, .y = 210.25F};

/**
 * @brief The positions of the four entities, one per type, of the test that
 * announces them all: each differs from the others, so an entity drawn with
 * another type's sprite is seen.
 */
constexpr game::Position SHIP_POSITION{.x = 10.0F, .y = 20.0F};
constexpr game::Position PLAYER_MISSILE_POSITION{.x = 30.0F, .y = 40.0F};
constexpr game::Position BYDO_POSITION{.x = 50.0F, .y = 60.0F};
constexpr game::Position ENEMY_MISSILE_POSITION{.x = 70.0F, .y = 80.0F};

/** @brief The lives of a player flying, as a WorldState of the tests lists
 * it. */
constexpr std::uint8_t FLYING_PLAYER_LIVES = 3;

/** @brief The number of the second player of a game. */
constexpr std::uint8_t SECOND_PLAYER_NUMBER = 2;

/**
 * @brief The player numbers just outside the players of a game, numbered 1
 * to 4: the one before the first and the one after the last.
 */
constexpr std::uint8_t BEFORE_THE_FIRST_PLAYER = 0;
constexpr std::uint8_t AFTER_THE_LAST_PLAYER = 5;

/** @brief The number of the game events the tests send. */
constexpr std::uint16_t AN_EVENT_NUMBER = 7;

/** @brief What a test reads when a history answers nothing: a position no
 * test expects. */
constexpr game::Position NO_HISTORY_POSITION{.x = -1.0e9F, .y = -1.0e9F};

/**
 * @brief Two states two ticks apart that list an entity at FIRST_POSITION then
 * at SECOND_POSITION, the tick between them, and where an entity lies at that
 * tick, halfway: (100 + 150.5) / 2 and (200 + 210.25) / 2.
 */
constexpr std::uint32_t FIRST_LISTING_TICK = 10;
constexpr std::uint32_t SECOND_LISTING_TICK = 12;
constexpr double TICK_BETWEEN_THE_LISTINGS = 11.0;
constexpr game::Position HALFWAY_BETWEEN_THE_POSITIONS{.x = 125.25F,
                                                       .y = 205.125F};

/** @brief The player of this client in the replicator tests, and another. */
constexpr std::uint8_t OWN_PLAYER = 1;
constexpr std::uint8_t ANOTHER_PLAYER = 2;

/** @brief The input that fired the missiles of the claim tests. */
constexpr std::uint32_t SOME_FIRING_INPUT = 9;

}  // namespace rtype::client::testing
