#pragma once

#include <chrono>

namespace rtype::network {

/** @brief A date of the casino, in milliseconds since 1970: the phases of a
 * round are timed to the millisecond, by the master and its clients alike. */
using MillisecondDate = std::chrono::sys_time<std::chrono::milliseconds>;

}  // namespace rtype::network
