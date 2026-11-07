#pragma once

#include <atomic>
#include <cstddef>
#include <optional>
#include <span>
#include <stop_token>
#include <string>
#include <vector>
#include "IConsoleInput.hpp"

/** @brief An input that hands the chunks its test gives, one per read, then
 * either ends, or waits for the stop as a terminal nobody types in. */
class ScriptedConsoleInput final : public rtype::server::IConsoleInput {
 public:
  /** @param chunks What each read hands, in order, each no longer than the
   * buffer. @param endsAfterChunks Whether the input ends once they are read;
   * otherwise a read waits for the stop. */
  ScriptedConsoleInput(std::vector<std::string> chunks, bool endsAfterChunks);

  [[nodiscard]] std::optional<std::size_t> read(
      std::span<char> buffer, const std::stop_token& stop) override;

  /** @returns Whether a read was woken by the stop. */
  [[nodiscard]] bool wokeOnStop() const;

 private:
  std::vector<std::string> chunks_;
  std::size_t next_{0};
  bool endsAfterChunks_;
  std::atomic<bool> wokeOnStop_{false};
};
