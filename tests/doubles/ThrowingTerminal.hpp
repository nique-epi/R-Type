#pragma once

#include <string_view>
#include "ITerminal.hpp"

/** @brief Terminal double that enters typing mode, then fails every write with
 * a StartRefusedException, as a terminal that cannot show the prompt. */
class ThrowingTerminal final : public rtype::server::ITerminal {
 public:
  [[nodiscard]] bool enterTypingMode() override;
  void leaveTypingMode() override;
  void write(std::string_view text) override;

  /** @returns Whether the terminal is in typing mode now. */
  [[nodiscard]] bool inTypingMode() const;

 private:
  bool typing_{false};
};
