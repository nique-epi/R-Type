#pragma once

#include <string>
#include <vector>

namespace rtype::client {

/** @brief The asset ids a screen needs, by kind. */
struct AssetList {
  // NOLINTBEGIN(readability-redundant-member-init)
  std::vector<std::string> textures{};
  std::vector<std::string> sounds{};
  std::vector<std::string> fonts{};
  std::vector<std::string> music{};
  // NOLINTEND(readability-redundant-member-init)
};

}  // namespace rtype::client
