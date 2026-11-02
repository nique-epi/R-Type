#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace rtype::engine {

/**
 * @brief What a part of a program declares to have a whole number it reads
 * from the configuration.
 */
struct IntegerSettingDefinition {
  /** @brief The name `set` gives the value to. */
  std::string name;
  /** @brief The value before any `set`; none when the setting starts empty
   * (allowsNone). */
  std::optional<std::int64_t> defaultValue{0};
  /** @brief The lowest value `set` accepts. */
  std::int64_t lowest{0};
  /** @brief The highest value `set` accepts. */
  std::int64_t highest{0};
  /** @brief One sentence on what the setting changes. */
  std::string help;
  /** @brief Whether `set <name> none` clears the value. */
  bool allowsNone{false};
};

}  // namespace rtype::engine
