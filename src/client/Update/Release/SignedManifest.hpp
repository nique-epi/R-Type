#pragma once

#include <string_view>

namespace rtype::client {

/** @brief The manifest of a release as received, and the signature received
 * with it; both must outlive the struct. */
struct SignedManifest {
  std::string_view text;
  std::string_view signature;
};

}  // namespace rtype::client
