#pragma once

/**
 * @brief A message whose move constructor and move assignment may throw.
 */
struct MessageWithThrowingMove {
  int value{0};

  MessageWithThrowingMove() = default;
  MessageWithThrowingMove(const MessageWithThrowingMove&) = default;
  MessageWithThrowingMove& operator=(const MessageWithThrowingMove&) = default;
  // NOLINTNEXTLINE(performance-noexcept-move-constructor)
  MessageWithThrowingMove(MessageWithThrowingMove&& other) noexcept(false);
  // NOLINTNEXTLINE(performance-noexcept-move-constructor)
  MessageWithThrowingMove& operator=(MessageWithThrowingMove&& other) noexcept(
      false);
  ~MessageWithThrowingMove() = default;
};
