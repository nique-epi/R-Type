#include "CommandInterpreter.hpp"
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "CommandDefinition.hpp"
#include "CommandHandler.hpp"
#include "CommandLine.hpp"
#include "CommandResult.hpp"
#include "EngineException.hpp"

namespace rtype::engine {

namespace {

CommandResult refusal(const std::string& command, std::string_view line,
                      const std::string& reason) {
  return CommandResult{.accepted = false,
                       .message = "command '" + command + "' refused line '" +
                                  std::string(line) + "': " + reason};
}

}  // namespace

void CommandInterpreter::add(CommandDefinition definition,
                             CommandHandler handler) {
  if (definitions_.contains(definition.name)) {
    throw DuplicateCommandException(definition.name);
  }
  handlers_.emplace(definition.name, std::move(handler));
  definitions_.emplace(definition.name, definition);
}

CommandResult CommandInterpreter::execute(std::string_view line) const {
  const std::vector<std::string> words = splitCommandLine(line);
  if (words.empty()) {
    return CommandResult{.accepted = true, .message = {}};
  }
  const auto handler = handlers_.find(words.front());
  if (handler == handlers_.end()) {
    return CommandResult{.accepted = false,
                         .message = "unknown command '" + words.front() +
                                    "' in line '" + std::string(line) + "'"};
  }
  try {
    handler->second(std::span<const std::string>{words}.subspan(1));
  } catch (const CommandRefusedException& refused) {
    return refusal(words.front(), line, refused.what());
  }
  return CommandResult{.accepted = true, .message = {}};
}

std::vector<std::string> CommandInterpreter::help() const {
  std::vector<std::string> lines;
  for (const auto& [name, definition] : definitions_) {
    const std::string usage =
        definition.arguments.empty() ? name : name + " " + definition.arguments;
    lines.push_back(usage + " - " + definition.help);
  }
  return lines;
}

}  // namespace rtype::engine
