#pragma once

#include <cstddef>
#include "EntityDestroyed.hpp"
#include "PlayerLeft.hpp"

namespace rtype::server::testing {

/** @brief The messages the tests guarantee or send once. */
constexpr network::PlayerLeft SECOND_PLAYER_LEFT{
    .playerNumber = 2, .reason = network::DepartureReason::Left};
constexpr network::EntityDestroyed MONSTER_KILLED{
    .entityId = 7, .reason = network::DestructionReason::Killed};

/** @brief The players of the fixture, by the index addPlayer() takes, and the
 * one who joins later. */
constexpr std::size_t FIRST_PLAYER = 0;
constexpr std::size_t SECOND_PLAYER = 1;
constexpr std::size_t LATE_PLAYER = 2;

}  // namespace rtype::server::testing
