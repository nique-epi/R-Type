#pragma once

namespace sf {
class RenderTarget;
}  // namespace sf

namespace rtype::engine {
class ComponentRegistry;
}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief The picture of the entities of a game: repainted from their
 * components, then laid over the playfield.
 *
 * The game screen paints it in its update and lays it in its draw; it never
 * touches the graphics card itself, so it can be tested without one.
 */
class IEntityLayer {
 public:
  virtual ~IEntityLayer() = default;

  /** @brief Repaints the layer with every entity that has a Position and a
   * Sprite. */
  virtual void paint(engine::ComponentRegistry& components) = 0;

  /**
   * @brief Lays the last painted picture over the playfield.
   *
   * @param target Target whose view shows the playfield in its logical units.
   */
  virtual void show(sf::RenderTarget& target) const = 0;
};

}  // namespace rtype::client
