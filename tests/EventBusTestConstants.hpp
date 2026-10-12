#pragma once

#include "Entity.hpp"

/**
 * @brief An entity the published events carry; no registry created it.
 */
constexpr rtype::engine::Entity FIRST_ENTITY{.index = 1, .generation = 0};

/**
 * @brief Another entity the published events carry.
 */
constexpr rtype::engine::Entity SECOND_ENTITY{.index = 2, .generation = 0};

/**
 * @brief A third entity the published events carry.
 */
constexpr rtype::engine::Entity THIRD_ENTITY{.index = 3, .generation = 0};

/**
 * @brief What the first subscriber records, to show when it was called.
 */
constexpr int FIRST_LABEL = 1;

/**
 * @brief What the second subscriber records, to show when it was called.
 */
constexpr int SECOND_LABEL = 2;

/**
 * @brief What the third subscriber records, to show when it was called.
 */
constexpr int THIRD_LABEL = 3;

/**
 * @brief Health of an entity that nothing has damaged.
 */
constexpr int FULL_HEALTH = 100;

/**
 * @brief Health a missile takes away from the entity it touches.
 */
constexpr int MISSILE_DAMAGE = 1;
