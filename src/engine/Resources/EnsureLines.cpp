#include "EnsureLines.hpp"
#include <algorithm>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "CommandLine.hpp"
#include "EngineException.hpp"
#include "HandOverRule.hpp"
#include "ResourceConstants.hpp"
#include "ResourceName.hpp"

namespace rtype::engine {

namespace {

/** @returns The resource @p line names, @p words being its words.
 * @throws What ensuredResources() throws for that line. */
std::string resourceOf(const std::string& line,
                       std::span<const std::string> words) {
  if (words.size() != ENSURE_LINE_WORDS) {
    throw InvalidEnsureLineException(line);
  }
  const std::string& name = words.back();
  requireResourceName(name);
  return name;
}

}  // namespace

bool isEnsureLine(std::span<const std::string> words) {
  return !words.empty() && words.front() == ENSURE_COMMAND_NAME;
}

HandOverRule ensureLineRule() {
  return [](std::span<const std::string> words) { return isEnsureLine(words); };
}

std::vector<std::string> ensuredResources(std::span<const std::string> lines) {
  std::vector<std::string> resources;
  for (const std::string& line : lines) {
    const std::vector<std::string> words = splitCommandLine(line);
    if (!isEnsureLine(words)) {
      continue;
    }
    std::string name = resourceOf(line, words);
    if (std::ranges::find(resources, name) == resources.end()) {
      resources.push_back(std::move(name));
    }
  }
  return resources;
}

}  // namespace rtype::engine
