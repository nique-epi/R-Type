#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "ChildProcess.hpp"
#include "InstallConstants.hpp"
#include "LaunchCommand.hpp"

/** @brief The environment of this process, which POSIX leaves to the program
 * to declare; the program started gets it. */
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,readability-redundant-declaration)
extern char** environ;

namespace rtype::client {

namespace {

/** @brief The type of a process id, taken from what getpid() returns: the
 * clang-tidy of the CI finds no header it accepts as the provider of pid_t. */
using ProcessId = decltype(::getpid());

std::vector<std::string> wordsOf(const LaunchCommand& command) {
  std::vector<std::string> words{command.program.string()};
  for (const std::filesystem::path& argument : command.arguments) {
    words.push_back(argument.string());
  }
  return words;
}

/** @returns The process started, or nothing when posix_spawn refused. */
std::optional<ProcessId> spawn(const LaunchCommand& command) {
  std::vector<std::string> words = wordsOf(command);
  std::vector<char*> argumentList;
  argumentList.reserve(words.size() + 1);
  for (std::string& word : words) {
    argumentList.push_back(word.data());
  }
  argumentList.push_back(nullptr);
  ProcessId child = 0;
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
  const std::optional<ProcessId> child = spawn(command);
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
