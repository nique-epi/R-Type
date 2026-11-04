#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <span>
#include <string_view>
#include "AssetsById.hpp"

namespace rtype::client {

class AssetLibrary;

/**
 * @brief The sounds of the client, read from the assets folder and kept as
 * long as the bank, so the sound module owns them and nothing outside Audio/
 * links the sound library of SFML.
 *
 * A sound is read once: loading an id the bank already holds keeps the first
 * buffer, and a lookup never reads the disk.
 */
class SoundBank {
 public:
  /**
   * @brief Reads every sound of @p soundIds the bank does not hold yet, from
   * the assets folder of @p library.
   * @throws AssetLoadingException naming every sound that could not be read,
   * once all of them were tried.
   */
  void load(const AssetLibrary& library, std::span<const char* const> soundIds);

  /** @returns Whether the bank holds no sound yet. */
  [[nodiscard]] bool isEmpty() const;

  /** @throws AssetNotLoadedException for a sound the bank does not hold. */
  [[nodiscard]] const sf::SoundBuffer& sound(std::string_view soundId) const;

 private:
  AssetsById<sf::SoundBuffer> sounds_;
};

}  // namespace rtype::client
