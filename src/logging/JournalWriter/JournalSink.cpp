#include "JournalSink.hpp"
#include <spdlog/details/log_msg.h>
#include <spdlog/details/thread_pool.h>
#include <spdlog/formatter.h>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include "LogLevel.hpp"
#include "QueuedLine.hpp"

namespace rtype::logging {

JournalSink::JournalSink(std::function<void(const QueuedLine&)> destination)
    : destination_(std::move(destination)) {}

void JournalSink::watch(spdlog::details::thread_pool& pool) {
  const std::scoped_lock lock{mutex_};
  pool_ = &pool;
  reportedLost_ = pool.discard_counter();
}

std::uint64_t JournalSink::flushCount() const { return flushes_.load(); }

void JournalSink::waitForFlushAfter(std::uint64_t seen) {
  std::unique_lock lock{mutex_};
  flushed_.wait(lock, [this, seen] { return flushes_.load() > seen; });
}

bool JournalSink::isJournalThread() const {
  return journalThread_.load() == std::this_thread::get_id();
}

void JournalSink::log(const spdlog::details::log_msg& message) {
  const std::scoped_lock lock{mutex_};
  journalThread_.store(std::this_thread::get_id());
  std::size_t lost = 0;
  if (pool_ != nullptr) {
    const std::size_t dropped = pool_->discard_counter();
    lost = dropped - reportedLost_;
    reportedLost_ = dropped;
  }
  destination_(QueuedLine{
      .level = static_cast<LogLevel>(message.level),
      .loggerName = std::string_view{message.logger_name.data(),
                                     message.logger_name.size()},
      .text = std::string_view{message.payload.data(), message.payload.size()},
      .time = message.time,
      .lostBefore = lost});
}

void JournalSink::flush() {
  {
    const std::scoped_lock lock{mutex_};
    journalThread_.store(std::this_thread::get_id());
    ++flushes_;
  }
  flushed_.notify_all();
}

void JournalSink::set_pattern([[maybe_unused]] const std::string& pattern) {}

void JournalSink::set_formatter(
    [[maybe_unused]] std::unique_ptr<spdlog::formatter> formatter) {}

}  // namespace rtype::logging
