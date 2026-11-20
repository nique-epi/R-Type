#pragma once

#include <string>
#include <vector>
#include "EmissionGroupDescription.hpp"
#include "LayerDescription.hpp"

namespace rtype::client {

/** @brief A background as its file describes it, before anything is drawn:
 * its layers, from the farthest to the nearest, the emission groups its
 * layers of particles name, and the music a game plays while it shows. */
struct BackgroundDescription {
  /** The name a level gives it: the name of its file, without `.toml`. */
  std::string name;
  std::vector<LayerDescription> layers;
  std::vector<EmissionGroupDescription> groups;
  /** The asset id of the music a game plays while this background shows,
   * such as `music/big_iron.mp3`; empty for the music of a game. */
  std::string music;
};

}  // namespace rtype::client
