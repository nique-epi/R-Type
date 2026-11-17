#pragma once

#include <spdlog/async_logger.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/details/thread_pool.h>
#include <spdlog/formatter.h>
#include <spdlog/sinks/sink.h>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include "QueuedLine.hpp"

namespace rtype::logging {

/**
 * @brief The spdlog sink of a JournalWriter: it runs on the thread of the
 * journal, turns each message into a QueuedLine for the destination, and
 * counts the flushes it does so a caller can wait for one. The destination
 * formats the line itself, so the pattern and the formatter spdlog may set are
 * ignored.
 */
class JournalSink final : public spdlog::sinks::sink {
 public:
  explicit JournalSink(std::function<void(const QueuedLine&)> destination);

  /** @brief Reads the lines @p pool dropped from now on; @p pool must outlive
   * the messages it hands this sink. */
  void watch(spdlog::details::thread_pool& pool);

  /** @returns How many flushes this sink did; it takes no lock, so a line
   * the destination is still writing never holds it up. */
  [[nodiscard]] std::uint64_t flushCount() const;

  /** @brief Waits until this sink did more than @p seen flushes. */
  void waitForFlushAfter(std::uint64_t seen);

  /** @returns Whether the caller is the thread of the journal; it takes no
   * lock, so the destination may ask it. */
  [[nodiscard]] bool isJournalThread() const;

  /** @brief Hands @p message to the destination, with the lines the queue
   * dropped since the previous one. */
  void log(const spdlog::details::log_msg& message) override;

  /** @brief Counts the flush and wakes the callers that wait for it. */
  void flush() override;

  /** @brief Ignored: the destination formats the line. */
  void set_pattern(const std::string& pattern) override;

  /** @brief Ignored: the destination formats the line. */
  void set_formatter(std::unique_ptr<spdlog::formatter> formatter) override;

 private:
  std::function<void(const QueuedLine&)> destination_;
  std::mutex mutex_;
  spdlog::details::thread_pool* pool_{nullptr};
  std::size_t reportedLost_{0};
  std::atomic<std::uint64_t> flushes_{0};
  std::condition_variable flushed_;
  std::atomic<std::thread::id> journalThread_;
};

}  // namespace rtype::logging
