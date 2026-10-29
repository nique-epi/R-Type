#pragma once

#include <array>
#include <vector>
#include "ComponentRegistry.hpp"
#include "DrawCall.hpp"
#include "Entity.hpp"
#include "IDrawSurface.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "SpriteAppearance.hpp"

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
  /** @brief Adds a draw call for every entity that has a Position and a
   * Sprite of a drawing layer. */
  void collectSprites(engine::ComponentRegistry& components);

  /** @brief Adds a draw call for every entity that has a Position and a
   * visible Caption of a drawing layer. */
  void collectCaptions(engine::ComponentRegistry& components);

  /** @returns How the effects on an entity change its drawing, for a frame
   * @p frameHeight rows tall. */
  [[nodiscard]] static SpriteAppearance lookOf(
      engine::ComponentRegistry& components, engine::Entity entity,
      int frameHeight);

  /** @brief Draws the sprite or writes the caption of a call. */
  static void draw(IDrawSurface& surface, const DrawCall& call);

  /** @brief Draws the sprite of a call, offset as its look says: as revealed
   * while a Reveal is on it, the arrival winning over a roll; rolled while
   * it rolls; whole otherwise. */
  static void drawSprite(IDrawSurface& surface, const DrawCall& call);

  /** @brief Draws the rows of a sprite a Reveal shows so far, at their place
   * in the frame, then the leading line under them; nothing, not even the
   * line, while it shows no row. */
  static void drawRevealed(IDrawSurface& surface, const DrawCall& call,
                           game::Position center);

  /** @brief Adds, in the effects layer under the ships, a ghost draw call at
   * the AFTERIMAGE_AGES frames before of every ship that rolls or dashes,
   * except a ship still arriving (Reveal). */
  void collectAfterimages(engine::ComponentRegistry& components);

  /** @brief Draws a rolling sprite row by row: round(height x |cos|) rows of
   * its frame, ROLL_THINNEST_ROWS at least, upside down and shaded once its
   * belly shows, unless a hit or an invulnerability already tones it. The
   * outline of an invulnerable ship is left out while it rolls. */
  static void drawRolled(IDrawSurface& surface, const DrawCall& call,
                         game::Position center);

  std::array<std::vector<DrawCall>, LAYER_COUNT> drawCallsByLayer_;
};

}  // namespace rtype::client
