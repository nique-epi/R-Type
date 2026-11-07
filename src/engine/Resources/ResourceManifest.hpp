#pragma once

#include <string>
#include <vector>

namespace rtype::engine {

/** @brief What the `resource.toml` of a resource says of it. */
struct ResourceManifest {
  /** @brief The name of the resource, the name of its folder. */
  std::string name;
  /** @brief What the resource adds, one line for the journal. */
  std::string description;
  /** @brief The resources it needs, in the order of the file. */
  std::vector<std::string> dependencies;
};

}  // namespace rtype::engine
