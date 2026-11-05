#pragma once

#include <chrono>
#include <stop_token>

namespace rtype::concurrency {

/**
 * @brief Sleeps @p duration on the calling thread, or less once @p stop is
 * requested, before the call or during it.
 *
 * std::this_thread::sleep_for cannot be woken: a thread that waits seconds
 * between two tries would keep the program from stopping that long. This waits
 * on a std::condition_variable_any instead, whose wait_for wakes when the stop
 * is requested ([thread.condvarany.intwait] of C++20); nothing else notifies
 * it, so it only wakes for the stop or once @p duration passed.
 */
void sleepUnlessStopped(const std::stop_token& stop,
                        std::chrono::milliseconds duration);

}  // namespace rtype::concurrency
