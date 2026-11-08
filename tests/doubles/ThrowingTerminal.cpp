#include "ThrowingTerminal.hpp"
#include <string_view>
#include "StartRefusedException.hpp"

bool ThrowingTerminal::enterTypingMode() {
  typing_ = true;
  return typing_;
}

void ThrowingTerminal::leaveTypingMode() { typing_ = false; }

void ThrowingTerminal::write([[maybe_unused]] std::string_view text) {
  throw StartRefusedException();
}

bool ThrowingTerminal::inTypingMode() const { return typing_; }
