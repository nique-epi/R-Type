#pragma once

#include <string_view>

namespace rtype::network {

/** @brief A secret a caller presents, and the one it must equal. */
struct SecretComparison {
  /** @brief What the caller sent; any text, the caller chooses it. */
  std::string_view presented;
  /** @brief What the program holds. */
  std::string_view expected;
};

}  // namespace rtype::network
