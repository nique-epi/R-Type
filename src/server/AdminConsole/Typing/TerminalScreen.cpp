#include "TerminalScreen.hpp"
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "AdminConsoleConstants.hpp"
#include "ConsoleChunk.hpp"
#include "ITerminal.hpp"
#include "LineEdit.hpp"

namespace rtype::server {

TerminalScreen::TerminalScreen(ITerminal& terminal) : terminal_(&terminal) {}

void TerminalScreen::open() {
  const std::lock_guard<std::mutex> lock(mutex_);
  terminal_->write(std::string{CONSOLE_READY_TEXT} + std::string{ROW_END} +
                   std::string{CONSOLE_PROMPT});
}

ConsoleChunk TerminalScreen::feed(std::span<const char> bytes) {
  const std::lock_guard<std::mutex> lock(mutex_);
  LineEdit edit = editor_.feed(bytes);
  terminal_->write(edit.echo);
  return std::move(edit.chunk);
}

ConsoleChunk TerminalScreen::finish() {
  const std::lock_guard<std::mutex> lock(mutex_);
  return editor_.finish();
}

void TerminalScreen::writeAbove(std::string_view line) {
  const std::lock_guard<std::mutex> lock(mutex_);
  std::string text{ERASE_ROW};
  text += line;
  text += ROW_END;
  text += CONSOLE_PROMPT;
  text += editor_.typed();
  terminal_->write(text);
}

void TerminalScreen::close() {
  const std::lock_guard<std::mutex> lock(mutex_);
  terminal_->write(ERASE_ROW);
}

}  // namespace rtype::server
