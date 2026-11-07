#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <stop_token>

namespace rtype::server {

/**
 * @brief Where the administration console reads what is typed: the standard
 * input of the supervisor, or what a test scripts.
 */
class IConsoleInput {
 public:
  virtual ~IConsoleInput() = default;

  /**
   * @brief Waits until bytes can be read, then reads some into @p buffer.
   * Called from the AdminConsole thread only.
   * @returns How many bytes it read, 1 to the size of @p buffer; nothing when
   * the input ended (closed, or failing for good) or when @p stop was
   * requested, which wakes the wait.
   */
  [[nodiscard]] virtual std::optional<std::size_t> read(
      std::span<char> buffer, const std::stop_token& stop) = 0;
};

}  // namespace rtype::server
