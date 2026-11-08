#include "ConsoleAnswers.hpp"
#include <string_view>
#include "IConsoleOutput.hpp"
#include "TerminalScreen.hpp"

namespace rtype::server {

ConsoleAnswers::ConsoleAnswers(IConsoleOutput& output) : output_(&output) {}

void ConsoleAnswers::write(std::string_view line) {
  if (screen_ != nullptr) {
    screen_->writeAbove(line);
    return;
  }
  output_->write(line);
}

void ConsoleAnswers::showOn(TerminalScreen* screen) { screen_ = screen; }

}  // namespace rtype::server
