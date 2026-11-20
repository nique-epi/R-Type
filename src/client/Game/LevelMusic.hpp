#pragma once

#include <optional>
#include <string>
#include "IAudioService.hpp"
#include "ISky.hpp"
#include "LevelStatus.hpp"

namespace rtype::client {

/**
 * @brief Plays the music the background of the biome a level shows names,
 * such as Big Iron on the maps of Deseret, and the music of a game for a
 * biome that names none. The server repeats its LevelStatus, so it asks the
 * audio service only when the music changes: a boss music that plays is never
 * stopped by a status of the same biome.
 */
class LevelMusic {
 public:
  /** @param sky Answers the music of a biome; @param audio Plays it. Both
   * must outlive it. */
  LevelMusic(const ISky& sky, IAudioService& audio);

  /** @brief Plays the music of the biome @p status names, unless it is the
   * music it played last. */
  void follow(const network::LevelStatus& status);

 private:
  const ISky* sky_;
  IAudioService* audio_;
  std::optional<std::string> played_;
};

}  // namespace rtype::client
