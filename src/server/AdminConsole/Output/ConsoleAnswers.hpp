#pragma once

#include <string_view>
#include "IConsoleOutput.hpp"
#include "TerminalScreen.hpp"

namespace rtype::server {

/**
 * @brief Where the commands of the console answer: the output the module was
 * given, or, while the console reads a terminal in typing mode, above the
 * typed line of its screen. The module calls showOn() in start() and stop(),
 * when no command runs.
 */
class ConsoleAnswers final : public IConsoleOutput {
 public:
  /** @param output Where it answers without a screen; it must outlive it. */
  explicit ConsoleAnswers(IConsoleOutput& output);

  void write(std::string_view line) override;

  /** @brief Answers on @p screen from now on; on the output again when it is
   * null. */
  void showOn(TerminalScreen* screen);

 private:
  IConsoleOutput* output_;
  TerminalScreen* screen_{nullptr};
};

}  // namespace rtype::server
