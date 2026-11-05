#include "MessageWithThrowingMove.hpp"
#include <utility>

// NOLINTNEXTLINE(performance-noexcept-move-constructor)
MessageWithThrowingMove::MessageWithThrowingMove(
    MessageWithThrowingMove&& other) noexcept(false)
    : value(std::move(other).value) {}

// NOLINTNEXTLINE(performance-noexcept-move-constructor)
MessageWithThrowingMove& MessageWithThrowingMove::operator=(
    MessageWithThrowingMove&& other) noexcept(false) {
  value = std::move(other).value;
  return *this;
}
