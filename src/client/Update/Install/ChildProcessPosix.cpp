#include <spawn.h>
#include <sys/wait.h>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "ChildProcess.hpp"
#include "InstallConstants.hpp"
#include "LaunchCommand.hpp"

/** @brief The environment of this process, which POSIX leaves to the program
 * to declare; the program started gets it. */
extern char** environ;  // NOLINT(readability-redundant-declaration)

namespace rtype::client {

namespace {

std::vector<std::string> wordsOf(const LaunchCommand& command) {
  std::vector<std::string> words{command.program.string()};
  for (const std::filesystem::path& argument : command.arguments) {
    words.push_back(argument.string());
  }
  return words;
}

/** @returns The process started, or nothing when posix_spawn refused. */
std::optional<pid_t> spawn(const LaunchCommand& command) {
  std::vector<std::string> words = wordsOf(command);
  std::vector<char*> argumentList;
  argumentList.reserve(words.size() + 1);
  for (std::string& word : words) {
    argumentList.push_back(word.data());
  }
  argumentList.push_back(nullptr);
  pid_t child = 0;
  if (posix_spawn(&child, words.front().c_str(), nullptr, nullptr,
                  argumentList.data(), environ) != 0) {
    return std::nullopt;
  }
  return child;
}

}  // namespace

bool launchDetached(const LaunchCommand& command) {
  return spawn(command).has_value();
}

int runToEnd(const LaunchCommand& command) {
  const std::optional<pid_t> child = spawn(command);
  if (!child.has_value()) {
    return LAUNCH_FAILED_STATUS;
  }
  int status = 0;
  if (waitpid(*child, &status, 0) != *child || !WIFEXITED(status)) {
    return LAUNCH_FAILED_STATUS;
  }
  return WEXITSTATUS(status);
}

}  // namespace rtype::client
