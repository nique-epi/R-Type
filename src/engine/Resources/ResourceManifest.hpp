#pragma once

#include <string>
#include <vector>

namespace rtype::engine {

/** @brief What the `resource.toml` of a resource says of it. */
struct ResourceManifest {
  /** @brief What the resource adds, one line for the host who starts it. */
  std::string description;
  /** @brief The resources it needs, in the order of the file. */
  std::vector<std::string> dependencies;
};

}  // namespace rtype::engine
