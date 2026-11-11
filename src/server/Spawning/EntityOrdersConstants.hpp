#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::server {

constexpr std::string_view ENTITY_ORDERS_LOGGER_NAME = "EntityOrders";

/** @brief The bodies the scripts may create in one tick: a whole volley of a
 * boss fits, a loop that creates bodies without end does not. */
constexpr std::size_t MAXIMUM_SPAWN_ORDERS_PER_TICK = 256;

/** @brief The bodies the scripts may remove in one tick. */
constexpr std::size_t MAXIMUM_REMOVAL_ORDERS_PER_TICK = 256;

}  // namespace rtype::server
