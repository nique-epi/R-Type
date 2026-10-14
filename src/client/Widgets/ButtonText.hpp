#pragma once

#include <string>

namespace rtype::client {

/**
 * @brief What a button says: its label, and the line the menu writes under
 * its buttons while it is selected, empty for none.
 */
struct ButtonText {
  std::string label;
  // NOLINTNEXTLINE(readability-redundant-member-init)
  std::string description{};
};

}  // namespace rtype::client
