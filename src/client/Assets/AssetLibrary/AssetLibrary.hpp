#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include "AssetKind.hpp"
#include "AssetList.hpp"
#include "AssetLoadResult.hpp"

namespace rtype::client {

/** @brief Assets of one kind, found by asset id. */
template <typename Asset>
using AssetsById = std::map<std::string, Asset, std::less<>>;

/** @brief The loaded assets of an assets folder, each loaded once and found by
 * id without reading the disk again. */
class AssetLibrary {
 public:
  explicit AssetLibrary(std::filesystem::path folder);

  AssetLibrary(const AssetLibrary&) = delete;
  AssetLibrary& operator=(const AssetLibrary&) = delete;
  AssetLibrary(AssetLibrary&&) = delete;
  AssetLibrary& operator=(AssetLibrary&&) = delete;
  ~AssetLibrary() = default;

  [[nodiscard]] const std::filesystem::path& folder() const;

  /** @brief The file an asset id names in the folder. */
  [[nodiscard]] std::filesystem::path fileOf(std::string_view assetId) const;

  /** @brief Loads an asset unless it is already loaded; music is only located,
   * since it is streamed when it plays. */
  [[nodiscard]] AssetLoadResult load(AssetKind kind, std::string_view assetId);

  /** @brief Releases every loaded asset the list does not name. */
  void releaseAllExcept(const AssetList& kept);

  [[nodiscard]] const sf::Texture& texture(std::string_view assetId) const;
  [[nodiscard]] const sf::SoundBuffer& sound(std::string_view assetId) const;
  [[nodiscard]] const sf::Font& font(std::string_view assetId) const;
  [[nodiscard]] const std::filesystem::path& musicFile(
      std::string_view assetId) const;

 private:
  std::filesystem::path folder_;
  AssetsById<sf::Texture> textures_;
  AssetsById<sf::SoundBuffer> sounds_;
  AssetsById<sf::Font> fonts_;
  AssetsById<std::filesystem::path> musicFiles_;
};

}  // namespace rtype::client
