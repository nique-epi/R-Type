#pragma once

#include <chrono>

namespace rtype::client::testing {

/** @brief The time each run of the stages of the client stands for: one frame
 * at 60 a second. */
constexpr std::chrono::milliseconds CONNECTION_FRAME{16};

/** @brief How long a test lets the server map read the master before it
 * gives up: well past the timeouts of a master that does not answer. */
constexpr std::chrono::seconds LISTING_DEADLINE{10};

/** @brief How long a test waits between two runs of the stages, so the thread
 * of the MasterLink gets to answer. */
constexpr std::chrono::milliseconds LISTING_POLL_STEP{10};

}  // namespace rtype::client::testing
