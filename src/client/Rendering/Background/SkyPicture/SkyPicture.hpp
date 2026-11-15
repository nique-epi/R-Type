#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "EnlargedPicture.hpp"
#include "PlanePlacement.hpp"

namespace rtype::client {

/**
 * @brief The picture of the sky, made of planes painted apart and laid back
 * to front, each moved right by a fraction of a pixel of its own.
 *
 * Each plane is painted on whole background pixels, on a canvas PLANE_MARGIN
 * columns wider on each side. To be shown, the planes are laid on one
 * EnlargedPicture, each moved by its fraction rounded to a pixel of that
 * enlarged picture: a slow plane moves by a pixel of the screen at a time,
 * not by a background pixel, and its pixels stay square, equal and sharp.
 * Where no plane paints, the sky shows SKY_COLOR.
 */
class SkyPicture {
 public:
  /**
   * @param planeCount How many planes the sky is made of.
   * @throws RenderTextureNotCreatedException when a plane or the flat
   * picture cannot get its render texture.
   */
  explicit SkyPicture(std::size_t planeCount);

  /**
   * @brief Clears the plane @p plane to transparent, to be laid where
   * @p placement says.
   *
   * @returns Where the plane is painted, in background pixels.
   */
  [[nodiscard]] sf::RenderTexture& paintPlane(std::size_t plane,
                                              PlanePlacement placement);

  /** @brief Leaves the plane @p plane out of the picture until it is painted
   * again. */
  void leavePlaneEmpty(std::size_t plane);

  /**
   * @brief Lays the planes over the whole playfield.
   *
   * @param target Target whose view shows the playfield in its logical units.
   * @throws RenderTextureNotCreatedException when the enlarged picture cannot
   * get a render texture of the size the target needs.
   */
  void show(sf::RenderTarget& target);

  /** @returns The picture show() laid last, at the size of the background, as
   * the source of the halo. It stays valid until the next call. */
  [[nodiscard]] const sf::Texture& flatPicture();

 private:
  /** @brief Lays every painted plane on @p picture, cleared to SKY_COLOR,
   * which is the background enlarged @p factor times. */
  void layPlanes(sf::RenderTexture& picture, unsigned int factor);

  std::vector<sf::RenderTexture> planes_;
  std::vector<std::optional<float>> shifts_;
  sf::RenderTexture flatPicture_;
  EnlargedPicture enlarged_;
};

}  // namespace rtype::client
