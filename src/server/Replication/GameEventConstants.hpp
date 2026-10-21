#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The number of the first GameEvent of a game, see the Game events
 * section of the protocol page.
 */
constexpr std::uint16_t FIRST_EVENT_NUMBER = 1;

/**
 * @brief Times a message is sent when a later state message corrects its
 * loss.
 */
constexpr std::size_t SINGLE_COPY = 1;

/**
 * @brief Times a message that no state message repeats is sent, the first one
 * included. Provisional, see the protocol page.
 */
constexpr std::size_t REPEATED_MESSAGE_COPIES = 3;

/**
 * @brief Time between a copy and the next one. Provisional, see the protocol
 * page.
 */
constexpr engine::Duration REPEATED_MESSAGE_INTERVAL =
    std::chrono::milliseconds(50);

/**
 * @brief The names the broadcaster gives the events in the warning it logs
 * when it skips one.
 */
constexpr std::string_view ENTITY_SPAWN_EVENT_NAME = "EntitySpawn";
constexpr std::string_view ENTITY_DESTRUCTION_EVENT_NAME = "EntityDestruction";
constexpr std::string_view ENTITY_DAMAGE_EVENT_NAME = "EntityDamage";
constexpr std::string_view PLAYER_HIT_EVENT_NAME = "PlayerHit";
constexpr std::string_view PLAYER_ELIMINATION_EVENT_NAME = "PlayerElimination";
constexpr std::string_view PLAYER_DEPARTURE_EVENT_NAME = "PlayerDeparture";

}  // namespace rtype::server
