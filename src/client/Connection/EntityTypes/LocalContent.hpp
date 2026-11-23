#pragma once

#include <vector>
#include "ContentFingerprint.hpp"
#include "NumberedEntityType.hpp"
#include "ShipArsenal.hpp"

namespace rtype::client {

/** @brief The client's own content: the fingerprint of the pack its assets
 * make, the types of its sheets and the guns of its ships. */
struct LocalContent {
  network::ContentFingerprint fingerprint;
  std::vector<game::NumberedEntityType> types;
  game::ShipArsenal arsenal;
};

}  // namespace rtype::client
