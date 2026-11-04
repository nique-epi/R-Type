#pragma once

#include <string_view>

namespace rtype::engine {

/**
 * @brief A launch option that gives a setting the value typed after it, as
 * `--database <file>` gives the `database` setting its file.
 */
struct ForcedSetting {
  /** @brief The option as it is typed, such as "--database". */
  std::string_view option;
  /** @brief The name the setting goes by in `set`. */
  std::string_view setting;
};

}  // namespace rtype::engine
