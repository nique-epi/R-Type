#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include "ValueBinding.hpp"

namespace rtype::client {

/** @brief What a Dropdown is built from. */
struct DropdownParts {
  /** @brief Texts of the choices, in order. */
  std::vector<std::string> choices;
  /** @brief Whether a choice can be taken, asked whenever needed. */
  std::function<bool(std::size_t)> available;
  /** @brief Order of the current choice. */
  ValueBinding<std::size_t> value;
  /** @brief The text the closed box shows in place of the current choice, or
   * nothing to show the choice; asked whenever the box is painted. Empty, the
   * box always shows the choice. */
  std::function<std::optional<std::string>()> imposedText;
};

}  // namespace rtype::client
