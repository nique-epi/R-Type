#pragma once

#include <chrono>

namespace rtype::master {

/** @brief A date of the casino, in milliseconds since 1970: the phases of a
 * round are timed to the millisecond. */
using MillisecondDate = std::chrono::sys_time<std::chrono::milliseconds>;

}  // namespace rtype::master
