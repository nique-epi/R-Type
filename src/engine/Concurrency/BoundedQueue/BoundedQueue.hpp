#pragma once

#include <concepts>
#include <cstddef>
#include <mutex>
#include <utility>
#include <vector>
#include "EngineException.hpp"

namespace rtype::engine {

/**
 * @brief Hands messages from one thread to another without ever making the
 * reader wait for the writer.
 *
 * Every call is safe from any thread. The capacity is reserved once, at
 * construction: push() never allocates beyond moving the message in. A full
 * queue discards its oldest message to make room for the new one, and counts
 * it.
 */
template <typename Message>
  requires std::default_initializable<Message> && std::movable<Message>
class BoundedQueue {
 public:
  /**
   * @throws InvalidQueueCapacityException if capacity is 0.
   */
  explicit BoundedQueue(std::size_t capacity) : slots_(capacity) {
    if (capacity == 0) {
      throw InvalidQueueCapacityException();
    }
  }

  /** @brief Adds the message after the others, discarding the oldest one when
   * the queue is full. */
  void push(Message message) {
    const std::scoped_lock lock{mutex_};
    if (size_ == slots_.size()) {
      slots_[head_] = std::move(message);
      head_ = (head_ + 1) % slots_.size();
      ++discardedCount_;
      return;
    }
    slots_[(head_ + size_) % slots_.size()] = std::move(message);
    ++size_;
  }

  /**
   * @brief Moves every waiting message, oldest first, to the end of
   * destination, and leaves the queue empty. Returns at once when nothing is
   * waiting.
   */
  void drainInto(std::vector<Message>& destination) {
    const std::scoped_lock lock{mutex_};
    for (std::size_t offset = 0; offset < size_; ++offset) {
      destination.push_back(
          std::move(slots_[(head_ + offset) % slots_.size()]));
    }
    size_ = 0;
  }

  /** @returns The number of messages discarded since the queue was created. */
  std::size_t discardedCount() const {
    const std::scoped_lock lock{mutex_};
    return discardedCount_;
  }

 private:
  mutable std::mutex mutex_;
  std::vector<Message> slots_;
  std::size_t head_{0};
  std::size_t size_{0};
  std::size_t discardedCount_{0};
};

}  // namespace rtype::engine
