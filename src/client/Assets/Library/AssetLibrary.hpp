#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>
#include <string_view>
#include "AssetKind.hpp"
#include "AssetList.hpp"
#include "AssetLoadResult.hpp"
#include "AssetsById.hpp"

namespace rtype::client {

/** @brief The loaded assets of an assets folder, each loaded once and found by
 * id without reading the disk again; the permanent ones are never released. */
class AssetLibrary {
 public:
  explicit AssetLibrary(std::filesystem::path folder);

  AssetLibrary(const AssetLibrary&) = delete;
  AssetLibrary& operator=(const AssetLibrary&) = delete;
  AssetLibrary(AssetLibrary&&) = delete;
  AssetLibrary& operator=(AssetLibrary&&) = delete;
  ~AssetLibrary() = default;

  [[nodiscard]] const std::filesystem::path& folder() const;

  /** @brief The file an asset id names in the folder; an id that is a whole
   * path, as the sprite sheets of a content pack are, names that file. */
  [[nodiscard]] std::filesystem::path fileOf(std::string_view assetId) const;

  /** @brief Loads an asset unless it is already loaded; music is only located,
   * since it is streamed when it plays. */
  [[nodiscard]] AssetLoadResult load(AssetKind kind, std::string_view assetId);

  [[nodiscard]] bool isLoaded(AssetKind kind, std::string_view assetId) const;

  /** @brief Tells whether every asset the list names is loaded; an empty list
   * is. */
  [[nodiscard]] bool isLoaded(const AssetList& assets) const;

  /** @brief Makes the listed assets permanent: no release drops them, whether
   * they are loaded yet or not. */
  void keepPermanently(const AssetList& permanent);

  /** @brief Releases every loaded asset that is neither in the list nor
   * permanent. */
  void releaseAllExcept(const AssetList& kept);

  [[nodiscard]] const sf::Texture& texture(std::string_view assetId) const;

  /** @returns The texture, or nullptr when the id names no loaded texture;
   * unlike texture(), it never throws. */
  [[nodiscard]] const sf::Texture* findTexture(std::string_view assetId) const;

  [[nodiscard]] const sf::Font& font(std::string_view assetId) const;
  [[nodiscard]] const std::filesystem::path& musicFile(
      std::string_view assetId) const;

 private:
  std::filesystem::path folder_;
  AssetsById<sf::Texture> textures_;
  AssetsById<sf::Font> fonts_;
  AssetsById<std::filesystem::path> musicFiles_;
  AssetList permanent_;
};

}  // namespace rtype::client
