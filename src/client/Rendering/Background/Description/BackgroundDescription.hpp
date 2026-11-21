#pragma once

#include <optional>
#include <string>
#include <vector>
#include "EmissionGroupDescription.hpp"
#include "LayerDescription.hpp"

namespace rtype::client {

/** @brief A background as its file describes it, before anything is drawn:
 * its layers, from the farthest to the nearest, the emission groups its
 * layers of particles name, and the musics a level and its bosses play while
 * it shows. */
struct BackgroundDescription {
  /** The name a level gives it: the name of its file, without `.toml`. */
  std::string name;
  std::vector<LayerDescription> layers;
  std::vector<EmissionGroupDescription> groups;
  /** The asset id of the music a game plays while the background shows, in
   * place of the music of a game; none for that music. */
  std::optional<std::string> music = std::nullopt;
  /** The asset id of the music a boss fought while it shows plays, in place
   * of the music of the bosses; none for the music of the level to play on,
   * or, without a music of the level, for the music of the bosses. */
  std::optional<std::string> bossMusic = std::nullopt;
};

}  // namespace rtype::client
