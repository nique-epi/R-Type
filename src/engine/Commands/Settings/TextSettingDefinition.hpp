#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace rtype::engine {

/**
 * @brief What a part of a program declares to have a text it reads from the
 * configuration.
 */
struct TextSettingDefinition {
  /** @brief The name `set` gives the value to. */
  std::string name;
  /** @brief The value before any `set`. */
  std::string defaultValue;
  /** @brief One sentence on what the setting changes. */
  std::string help;
  /** @brief Checks a value before it is kept and throws a
   * CommandRefusedException to refuse it; empty to accept any text. */
  std::function<void(std::string_view)> validate;
};

}  // namespace rtype::engine
