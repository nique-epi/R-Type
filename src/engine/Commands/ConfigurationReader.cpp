#include "ConfigurationReader.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "CommandInterpreter.hpp"
#include "CommandLine.hpp"
#include "CommandResult.hpp"
#include "ConfigurationReport.hpp"

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

  /** @brief Runs @p line; a refusal in the report names @p origin. */
  void run(const std::string& line, const std::string& origin) {
    const CommandResult result = interpreter_->execute(line);
    if (!result.accepted) {
      report_.refusals.push_back(origin + ": " + result.message);
    } else if (!splitCommandLine(line).empty()) {
      ++report_.executedLines;
    }
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
    runner.run(line, file.string() + ":" + std::to_string(lineNumber));
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
    runner.run(line, std::string(FORCED_LINE_ORIGIN));
  }
  return runner.finish();
}

}  // namespace rtype::engine
