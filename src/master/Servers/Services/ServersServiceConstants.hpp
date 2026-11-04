#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::master {

/**
 * @brief How many servers one address may create in CREATION_WINDOW: a host
 * creates one or two, so a script that creates more is refused until the hour
 * has passed.
 */
constexpr std::int64_t MAXIMUM_CREATIONS_PER_ADDRESS = 5;
constexpr std::chrono::seconds CREATION_WINDOW = std::chrono::hours{1};

}  // namespace rtype::master
