#include "JournalWriter.hpp"
#include <spdlog/async_logger.h>
#include <spdlog/common.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/details/thread_pool.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include "JournalSink.hpp"
#include "LogLevel.hpp"
#include "LoggingConstants.hpp"
#include "QueuedLine.hpp"

namespace rtype::logging {

JournalWriter::JournalWriter(std::function<void(const QueuedLine&)> destination)
    : sink_(std::make_shared<JournalSink>(std::move(destination))) {}

JournalWriter::~JournalWriter() {
  try {
    close();
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

void JournalWriter::queue(LogLevel level, std::string_view loggerName,
                          std::string_view text) {
  auto [pool, logger] = running();
  pool->post_log(
      std::move(logger),
      spdlog::details::log_msg{
          spdlog::string_view_t{loggerName.data(), loggerName.size()},
          static_cast<spdlog::level::level_enum>(level),
          spdlog::string_view_t{text.data(), text.size()}},
      spdlog::async_overflow_policy::discard_new);
}

void JournalWriter::flush() {
  std::shared_ptr<spdlog::details::thread_pool> pool;
  std::shared_ptr<spdlog::async_logger> logger;
  {
    const std::scoped_lock lock{mutex_};
    pool = pool_;
    logger = logger_;
  }
  if (pool == nullptr || sink_->isJournalThread()) {
    return;
  }
  const std::uint64_t seen = sink_->flushCount();
  pool->post_flush(std::move(logger), spdlog::async_overflow_policy::block);
  sink_->waitForFlushAfter(seen);
}

void JournalWriter::close() {
  if (sink_->isJournalThread()) {
    return;
  }
  std::shared_ptr<spdlog::details::thread_pool> pool;
  {
    const std::scoped_lock lock{mutex_};
    pool = std::move(pool_);
    logger_.reset();
  }
  // The destructor of the pool queues its stop after the waiting lines, which
  // the thread writes before it ends, then joins it.
  pool.reset();
}

std::pair<std::shared_ptr<spdlog::details::thread_pool>,
          std::shared_ptr<spdlog::async_logger>>
JournalWriter::running() {
  const std::scoped_lock lock{mutex_};
  if (pool_ == nullptr) {
    pool_ = std::make_shared<spdlog::details::thread_pool>(
        JOURNAL_QUEUE_CAPACITY, JOURNAL_THREAD_COUNT);
    sink_->watch(*pool_);
    logger_ = std::make_shared<spdlog::async_logger>(
        std::string{LOGGING_LOGGER_NAME}, sink_, pool_,
        spdlog::async_overflow_policy::discard_new);
  }
  return {pool_, logger_};
}

}  // namespace rtype::logging
