#pragma once

#include <mutex>
#include <string>
#include <string_view>
#include "ITerminal.hpp"

/** @brief Terminal double that keeps everything written to it, and enters
 * typing mode only when the test says it is a terminal. The reader of the
 * keyboard and the simulation thread may write at once. */
class RecordingTerminal final : public rtype::server::ITerminal {
 public:
  /** @param isTerminal Whether enterTypingMode() succeeds. */
  explicit RecordingTerminal(bool isTerminal);

  [[nodiscard]] bool enterTypingMode() override;
  void leaveTypingMode() override;
  void write(std::string_view text) override;

  /** @returns Everything written so far, in order, as one text. */
  [[nodiscard]] std::string written() const;

  /** @returns Whether the terminal is in typing mode now. */
  [[nodiscard]] bool inTypingMode() const;

 private:
  mutable std::mutex mutex_;
  std::string written_;
  bool isTerminal_;
  bool typing_{false};
};
