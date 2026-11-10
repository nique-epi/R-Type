#pragma once

#include <string_view>

namespace rtype::client {

/** @brief The feed of a release as received, and the signature received with
 * it; both must outlive the struct. */
struct SignedFeed {
  std::string_view text;
  std::string_view signature;
};

}  // namespace rtype::client
