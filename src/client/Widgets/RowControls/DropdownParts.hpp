#pragma once

#include <cstddef>
#include <functional>
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
};

}  // namespace rtype::client
