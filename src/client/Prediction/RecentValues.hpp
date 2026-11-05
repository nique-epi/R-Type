#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <span>

namespace rtype::client {

/**
 * @brief The newest @p Capacity values pushed to it, oldest first: once it is
 * full, each push forgets the oldest value.
 */
template <typename Value, std::size_t Capacity>
class RecentValues {
 public:
  /** @brief Keeps @p value as the newest, forgetting the oldest when full. */
  void push(Value value) {
    if (count_ == Capacity) {
      std::ranges::rotate(values_, values_.begin() + 1);
      --count_;
    }
    values_[count_] = value;
    ++count_;
  }

  /** @returns The values kept, oldest first. */
  [[nodiscard]] std::span<const Value> values() const {
    return {values_.data(), count_};
  }

 private:
  std::array<Value, Capacity> values_{};
  std::size_t count_{0};
};

}  // namespace rtype::client
