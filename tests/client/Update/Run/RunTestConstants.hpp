#pragma once

#include <chrono>

namespace rtype::client::testing {

/** @brief How long a test waits for a thread, and how often it looks. */
constexpr std::chrono::seconds THREAD_WAIT{5};
constexpr std::chrono::milliseconds THREAD_POLL{1};

/** @brief The share a download reports once whole. */
constexpr int WHOLE_DOWNLOAD = 100;

}  // namespace rtype::client::testing
