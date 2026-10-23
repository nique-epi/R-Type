#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstdint>
#include <optional>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "IDrawSurface.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "SpriteTone.hpp"

namespace rtype::game {

struct Sprite;

}  // namespace rtype::game

namespace rtype::client {

struct Caption;

/**
 * @brief Draws every entity that has a Position and a Sprite, and writes
 * every visible Caption, layer by layer.
 *
 * Besides these, it reads how the effects change the drawing of an entity:
 * its HitFlash for its tone, its Invulnerability for a dithered picture and,
 * with a PilotShip, an outline, its Nudge for a few pixels of offset, its
 * Reveal for the rows a ship arriving shows. Whatever else an entity carries
 * (health, damage) is invisible to it. Inside a layer, entities are drawn by
 * ascending entity index, so the same registry always gives the same
 * sequence. An entity whose layer is not one of the drawing layers is not
 * drawn.
 */
class RenderSystem {
 public:
  /**
   * @brief Draws one frame, from the background layer to the interface layer.
   */
  void render(engine::ComponentRegistry& components, IDrawSurface& surface);

 private:
  /** @brief How an entity is drawn beyond its sprite: its tone and outline,
   * how many pixels of the canvas away from its Position, and how many rows
   * of its frame show, from the top, with or without a leading line. */
  struct SpriteLook {
    SpriteTone tone{};
    std::optional<sf::Color> outline;
    sf::Vector2i offset;
    std::optional<int> shownRows;
    bool leadingLine = false;
  };

  /** @brief One sprite or one caption to draw, in its layer. */
  struct DrawCall {
    std::uint32_t entityIndex{};
    game::Position position{};
    const game::Sprite* sprite{};
    const Caption* caption{};
    SpriteLook look;
  };

  /** @brief Adds a draw call for every entity that has a Position and a
   * Sprite of a drawing layer. */
  void collectSprites(engine::ComponentRegistry& components);

  /** @brief Adds a draw call for every entity that has a Position and a
   * visible Caption of a drawing layer. */
  void collectCaptions(engine::ComponentRegistry& components);

  /** @returns How the effects on an entity change its drawing, for a frame
   * @p frameHeight rows tall. */
  [[nodiscard]] static SpriteLook lookOf(engine::ComponentRegistry& components,
                                         engine::Entity entity,
                                         int frameHeight);

  /** @brief Draws the sprite or writes the caption of a call. */
  static void draw(IDrawSurface& surface, const DrawCall& call);

  /** @brief Draws the sprite of a call, offset as its look says, whole or as
   * revealed. */
  static void drawSprite(IDrawSurface& surface, const DrawCall& call);

  /** @brief Draws the rows of a sprite a Reveal shows so far, at their place
   * in the frame, then the leading line under them; nothing, not even the
   * line, while it shows no row. */
  static void drawRevealed(IDrawSurface& surface, const DrawCall& call,
                           game::Position center);

  std::array<std::vector<DrawCall>, LAYER_COUNT> drawCallsByLayer_;
};

}  // namespace rtype::client
