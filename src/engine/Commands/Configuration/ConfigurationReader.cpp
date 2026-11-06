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
#include "ConfigurationSource.hpp"
#include "ConfigurationStep.hpp"
#include "HandOverRule.hpp"
#include "PathText.hpp"

namespace rtype::engine {

namespace {

constexpr std::string_view FORCED_LINE_ORIGIN = "launch option";

/**
 * @brief Runs lines, or hands them over, and keeps in a report what happened
 * to each.
 */
class LineRunner {
 public:
  LineRunner(const CommandInterpreter& interpreter,
             const HandOverRule& handOver)
      : interpreter_(&interpreter), handOver_(&handOver) {}

  /**
   * @brief Hands @p line over when the rule says it belongs to another
   * program, runs it otherwise.
   * @returns The message of the refusal when the line was refused.
   */
  std::optional<std::string> run(const std::string& line) {
    const std::vector<std::string> words = splitCommandLine(line);
    if (handsOver(words)) {
      report_.handedOverLines.push_back(line);
      return std::nullopt;
    }
    const CommandResult result = interpreter_->execute(line);
    if (!result.accepted) {
      return result.message;
    }
    if (!words.empty()) {
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
  [[nodiscard]] bool handsOver(std::span<const std::string> words) const {
    return !words.empty() && *handOver_ && (*handOver_)(words);
  }

  const CommandInterpreter* interpreter_;
  const HandOverRule* handOver_;
  ConfigurationReport report_;
};

void runFile(std::ifstream& stream, const std::filesystem::path& file,
             LineRunner& runner) {
  std::string line;
  std::size_t lineNumber = 0;
  while (std::getline(stream, line)) {
    ++lineNumber;
    if (const std::optional<std::string> refusal = runner.run(line)) {
      runner.refuse(logging::genericText(file) + ":" +
                    std::to_string(lineNumber) + ": " + *refusal);
    }
  }
}

void runFileOf(const ConfigurationSource& source, LineRunner& runner) {
  if (source.file.empty()) {
    return;
  }
  std::ifstream stream{source.file};
  if (stream.is_open()) {
    runner.markFileFound();
    runFile(stream, source.file, runner);
  }
}

}  // namespace

ConfigurationReport readConfiguration(const ConfigurationSource& source,
                                      const CommandInterpreter& interpreter) {
  LineRunner runner{interpreter, source.handOver};
  runFileOf(source, runner);
  for (const std::string& line : source.forcedLines) {
    if (const std::optional<std::string> refusal = runner.run(line)) {
      runner.refuse(std::string(FORCED_LINE_ORIGIN) + ": " + *refusal);
    }
  }
  return runner.finish();
}

ConfigurationReport readConfiguration(const std::filesystem::path& file,
                                      std::span<const std::string> forcedLines,
                                      const CommandInterpreter& interpreter) {
  return readConfiguration(
      ConfigurationSource{
          .file = file,
          .forcedLines = {forcedLines.begin(), forcedLines.end()},
          .handOver = {}},
      interpreter);
}

ConfigurationStep configurationStep(ConfigurationSource source,
                                    ConfigurationReport& report) {
  return
      [source = std::move(source), &report](CommandInterpreter& interpreter) {
        report = readConfiguration(source, interpreter);
      };
}

ConfigurationStep configurationStep(std::filesystem::path file,
                                    std::vector<std::string> forcedLines,
                                    ConfigurationReport& report) {
  return configurationStep(
      ConfigurationSource{.file = std::move(file),
                          .forcedLines = std::move(forcedLines),
                          .handOver = {}},
      report);
}

}  // namespace rtype::engine
