#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "WholeNumberText.hpp"

namespace rtype::server {

/**
 * @returns The whole number @p text is made of, digits only.
 * @throws engine::InvalidLaunchOptionValueException naming @p option when it
 * is anything else, or does not fit in a Number.
 */
template <typename Number>
[[nodiscard]] Number wholeNumberFromText(std::string_view option,
                                         std::string_view text) {
  const std::optional<Number> number = network::wholeNumberIn<Number>(text);
  if (!number.has_value()) {
    throw engine::InvalidLaunchOptionValueException(std::string(option),
                                                    std::string(text));
  }
  return *number;
}

}  // namespace rtype::server
