#pragma once

#include <array>
#include "EntityType.hpp"

/** @brief Every entity type the server can announce. */
constexpr std::array ALL_ENTITY_TYPES{rtype::game::EntityType::Player,
                                      rtype::game::EntityType::Enemy,
                                      rtype::game::EntityType::PlayerMissile,
                                      rtype::game::EntityType::EnemyMissile};
