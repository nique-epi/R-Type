#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <string_view>
#include <utility>
#include "LogLevel.hpp"
#include "QueuedLine.hpp"

namespace spdlog {
class async_logger;
namespace details {
class thread_pool;
}  // namespace details
}  // namespace spdlog

namespace rtype::logging {

class JournalSink;

/**
 * @brief Hands lines to one thread of its own, which passes them, in the order
 * they were queued, to the destination given at construction: the caller never
 * waits for the disk.
 *
 * It is an spdlog thread pool of one thread (one thread keeps the order) and
 * JOURNAL_QUEUE_CAPACITY places reserved when it starts, with the discard_new
 * policy: when the queue is full, the new line is dropped and counted, and the
 * destination is told how many with the next line it gets, so the first lines
 * of an incident are kept. The thread starts with the first line and stops in
 * close(); a line queued after close() starts it again.
 *
 * Every call is safe from any thread, except flush() and close() from the
 * destination, which does nothing.
 */
class JournalWriter {
 public:
  /** @param destination called on the thread of the journal, one line at a
   * time; it must not throw. */
  explicit JournalWriter(std::function<void(const QueuedLine&)> destination);

  /** @brief Writes what is queued, then stops the thread. */
  ~JournalWriter();

  JournalWriter(const JournalWriter&) = delete;
  JournalWriter& operator=(const JournalWriter&) = delete;
  JournalWriter(JournalWriter&&) = delete;
  JournalWriter& operator=(JournalWriter&&) = delete;

  /** @brief Queues @p text, timed now; drops it when the queue is full. */
  void queue(LogLevel level, std::string_view loggerName,
             std::string_view text);

  /** @brief Waits until every line queued before the call reached the
   * destination. */
  void flush();

  /** @brief Writes what is queued, then joins the thread. */
  void close();

 private:
  /** @brief The pool and the logger the next line is queued on, started when
   * stopped. */
  [[nodiscard]] std::pair<std::shared_ptr<spdlog::details::thread_pool>,
                          std::shared_ptr<spdlog::async_logger>>
  running();

  std::shared_ptr<JournalSink> sink_;
  std::mutex mutex_;
  std::shared_ptr<spdlog::details::thread_pool> pool_;
  std::shared_ptr<spdlog::async_logger> logger_;
};

}  // namespace rtype::logging
