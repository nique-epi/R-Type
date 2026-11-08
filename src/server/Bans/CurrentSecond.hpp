#pragma once

#include <chrono>

namespace rtype::server {

/** @returns The time now on the system clock, to the second: the moment a ban
 * is judged at, since a ban ends on a whole second. */
[[nodiscard]] std::chrono::sys_seconds currentSecond();

}  // namespace rtype::server
