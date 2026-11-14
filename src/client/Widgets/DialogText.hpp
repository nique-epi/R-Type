#pragma once

#include <string_view>
#include <vector>

namespace rtype::client {

/** @brief What a dialog writes, which sizes it: its heading, its message, how
 * many rows it holds, and the labels of its buttons. */
struct DialogText {
  std::string_view heading;
  std::string_view message;
  int rowCount{};
  std::vector<std::string_view> barLabels;
};

}  // namespace rtype::client
