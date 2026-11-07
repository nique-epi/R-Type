#pragma once

#include <vector>
#include "ContentFingerprint.hpp"
#include "NumberedEntityType.hpp"

namespace rtype::client {

/** @brief The client's own content: the fingerprint of the pack its assets
 * make, and the types of its sheets. */
struct LocalContent {
  network::ContentFingerprint fingerprint;
  std::vector<game::NumberedEntityType> types;
};

}  // namespace rtype::client
