#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::master {

/** @returns @p date as the seconds since 1970 UTC a column of a table keeps. */
[[nodiscard]] std::int64_t epochSecondsOf(std::chrono::sys_seconds date);

/** @returns The date a column of a table keeps as @p seconds since 1970 UTC. */
[[nodiscard]] std::chrono::sys_seconds dateOfEpochSeconds(std::int64_t seconds);

}  // namespace rtype::master
