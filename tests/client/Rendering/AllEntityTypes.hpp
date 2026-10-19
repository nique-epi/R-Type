#pragma once

#include <array>
#include <cstdint>
#include "EntityType.hpp"

/** @brief Every entity type the server can announce. */
constexpr std::array ALL_ENTITY_TYPES{rtype::game::EntityType::Player,
                                      rtype::game::EntityType::Enemy,
                                      rtype::game::EntityType::PlayerMissile,
                                      rtype::game::EntityType::EnemyMissile};

/** @brief The first value past the last entity type, as a corrupted type
 * would carry. */
constexpr auto FIRST_VALUE_AFTER_THE_LAST_TYPE =
    static_cast<rtype::game::EntityType>(
        static_cast<std::uint8_t>(rtype::game::EntityType::EnemyMissile) + 1);
