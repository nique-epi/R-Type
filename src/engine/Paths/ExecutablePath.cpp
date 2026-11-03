#include "ExecutablePath.hpp"
#include <filesystem>
#include <optional>
#include "EngineException.hpp"

namespace rtype::engine {

std::filesystem::path executableFolder() {
  const std::optional<std::filesystem::path> executable = executablePath();
  if (!executable.has_value()) {
    throw UnknownExecutablePathException();
  }
  return executable->parent_path();
}

}  // namespace rtype::engine
