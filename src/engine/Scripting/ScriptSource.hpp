#pragma once

#include <string>

namespace rtype::engine {

/** @brief The text of a script and the name its errors give it, such as the
 * path of its file below its resource. */
struct ScriptSource {
  std::string name;
  std::string text;
};

}  // namespace rtype::engine
