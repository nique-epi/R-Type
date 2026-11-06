#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include "Logger.hpp"
#include "SmoothImage.hpp"
#include "SmoothImageSpot.hpp"

namespace rtype::client {

/**
 * @brief The smooth images of an interface picture, which are no part of its
 * pixels: display() keeps the list of those laid since clear(), and show()
 * lays them over the picture, each stretched over its area at the resolution
 * of the window, from a copy of its texture smoothed and given mipmaps, made
 * the first time its id is laid.
 */
class SmoothImageLayer {
 public:
  SmoothImageLayer();

  /** @brief Forgets the images laid since the last display(). */
  void clear();

  /** @brief Lays @p texture, known as @p assetId, on @p spot. */
  void lay(std::string_view assetId, const sf::Texture& texture,
           SmoothImageSpot spot);

  /** @brief Keeps the images laid since clear() for show(). */
  void display();

  /** @brief Lays the images of the last display() over @p target, in logical
   * units of the playfield. */
  void show(sf::RenderTarget& target) const;

 private:
  /** @returns The smoothed copy of @p texture, made the first time @p assetId
   * is laid: given mipmaps, so drawing it smaller than its size stays smooth;
   * without mipmaps, smoothed only, and the log says so. */
  [[nodiscard]] const sf::Texture& copyOf(std::string_view assetId,
                                          const sf::Texture& texture);

  logging::Logger logger_;
  std::map<std::string, sf::Texture, std::less<>> copies_;
  std::vector<SmoothImage> laid_;
  std::vector<SmoothImage> shown_;
};

}  // namespace rtype::client
