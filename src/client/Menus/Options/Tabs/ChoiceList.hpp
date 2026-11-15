#pragma once

#include <span>
#include <string>
#include <vector>

namespace rtype::client {

/** @brief The values a selector row cycles through, in the order it shows
 * them, and the text it shows for each, in the same order. */
template <typename Value>
struct ChoiceList {
  std::span<const Value> values;
  std::vector<std::string> texts;
};

}  // namespace rtype::client
