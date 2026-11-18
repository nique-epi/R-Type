#pragma once

#include <memory>
#include <vector>
#include "EmissionGroup.hpp"
#include "ILayerContent.hpp"

namespace rtype::client {

/** @brief What a background is made of once built: what each of its layers
 * shows, in the order of its description, and its emission groups, which
 * refer to the layers of particles they give birth in. */
struct BuiltLayers {
  std::vector<std::unique_ptr<ILayerContent>> contents;
  std::vector<EmissionGroup> groups;
};

}  // namespace rtype::client
