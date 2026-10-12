#pragma once

#include <string>
#include <vector>

namespace rtype::client {

/** @brief Asset ids by kind: the assets of the interface, or those of a
 * game. */
struct AssetList {
  // NOLINTBEGIN(readability-redundant-member-init)
  std::vector<std::string> textures{};
  std::vector<std::string> sounds{};
  std::vector<std::string> fonts{};
  std::vector<std::string> music{};
  // NOLINTEND(readability-redundant-member-init)
};

}  // namespace rtype::client
