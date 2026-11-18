#pragma once

#include <string>
#include <vector>
#include "EmissionGroupDescription.hpp"
#include "LayerDescription.hpp"

namespace rtype::client {

/** @brief A background as its file describes it, before anything is drawn:
 * its layers, from the farthest to the nearest, and the emission groups its
 * layers of particles name. */
struct BackgroundDescription {
  /** The name a level gives it: the name of its file, without `.toml`. */
  std::string name;
  std::vector<LayerDescription> layers;
  std::vector<EmissionGroupDescription> groups;
};

}  // namespace rtype::client
