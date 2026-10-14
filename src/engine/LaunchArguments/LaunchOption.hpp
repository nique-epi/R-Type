#pragma once

#include <functional>
#include <string_view>

namespace rtype::engine {

/**
 * @brief An option a program accepts at launch, and what reading it does.
 */
struct LaunchOption {
  /** @brief The option as it is typed, such as "-p". */
  std::string_view name;

  /** @brief Whether the argument after the option is its value. */
  bool takesValue;

  /**
   * @brief Called once each time the option is given: with its value, or
   * with an empty view for an option that takes none. It may throw to refuse
   * the value.
   */
  std::function<void(std::string_view value)> record;
};

}  // namespace rtype::engine
