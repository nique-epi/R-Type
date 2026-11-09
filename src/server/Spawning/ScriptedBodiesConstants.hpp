#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::server {

constexpr std::string_view SCRIPTED_BODIES_LOGGER_NAME = "ScriptedBodies";

/** @brief The bodies the scripts may create in one tick: a whole volley of a
 * boss fits, a loop that creates bodies without end does not. */
constexpr std::size_t MAXIMUM_SCRIPTED_SPAWNS_PER_TICK = 256;

/** @brief The bodies the scripts may remove in one tick. */
constexpr std::size_t MAXIMUM_SCRIPTED_REMOVALS_PER_TICK = 256;

}  // namespace rtype::server
