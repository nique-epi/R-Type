#include "ConfigurationReader.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "CommandInterpreter.hpp"
#include "CommandLine.hpp"
#include "CommandResult.hpp"
#include "ConfigurationReport.hpp"
#include "ConfigurationStep.hpp"

namespace rtype::engine {

namespace {

constexpr std::string_view FORCED_LINE_ORIGIN = "launch option";

/**
 * @brief Runs lines and keeps in a report what happened to each.
 */
class LineRunner {
 public:
  explicit LineRunner(const CommandInterpreter& interpreter)
      : interpreter_(&interpreter) {}

  /**
   * @brief Runs @p line.
   * @returns The message of the refusal when the line was refused.
   */
  std::optional<std::string> run(const std::string& line) {
    const CommandResult result = interpreter_->execute(line);
    if (!result.accepted) {
      return result.message;
    }
    if (!splitCommandLine(line).empty()) {
      ++report_.executedLines;
    }
    return std::nullopt;
  }

  /** @brief Keeps @p message, which names where the refused line came from. */
  void refuse(const std::string& message) {
    report_.refusals.push_back(message);
  }

  void markFileFound() { report_.fileFound = true; }

  ConfigurationReport finish() { return std::move(report_); }

 private:
  const CommandInterpreter* interpreter_;
  ConfigurationReport report_;
};

void runFile(std::ifstream& stream, const std::filesystem::path& file,
             LineRunner& runner) {
  std::string line;
  std::size_t lineNumber = 0;
  while (std::getline(stream, line)) {
    ++lineNumber;
    if (const std::optional<std::string> refusal = runner.run(line)) {
      runner.refuse(file.string() + ":" + std::to_string(lineNumber) + ": " +
                    *refusal);
    }
  }
}

}  // namespace

ConfigurationReport readConfiguration(const std::filesystem::path& file,
                                      std::span<const std::string> forcedLines,
                                      const CommandInterpreter& interpreter) {
  LineRunner runner{interpreter};
  std::ifstream stream{file};
  if (stream.is_open()) {
    runner.markFileFound();
    runFile(stream, file, runner);
  }
  for (const std::string& line : forcedLines) {
    if (const std::optional<std::string> refusal = runner.run(line)) {
      runner.refuse(std::string(FORCED_LINE_ORIGIN) + ": " + *refusal);
    }
  }
  return runner.finish();
}

ConfigurationStep configurationStep(std::filesystem::path file,
                                    std::vector<std::string> forcedLines,
                                    ConfigurationReport& report) {
  return [file = std::move(file), forcedLines = std::move(forcedLines),
          &report](CommandInterpreter& interpreter) {
    report = readConfiguration(file, forcedLines, interpreter);
  };
}

}  // namespace rtype::engine
