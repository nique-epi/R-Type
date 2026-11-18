#pragma once

#include <chrono>

namespace rtype::client {

/** @brief How long the thread of a link to the master sleeps before it looks
 * again whether it must stop, when no ask comes; a stop wakes it at once. */
constexpr std::chrono::seconds MASTER_ASK_WAIT{1};

}  // namespace rtype::client
