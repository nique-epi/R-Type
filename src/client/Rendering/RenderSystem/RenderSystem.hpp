#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "ComponentRegistry.hpp"
#include "IDrawSurface.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"

namespace rtype::client {

/**
 * @brief Draws every entity that has a Position and a Sprite, layer by layer.
 *
 * It reads these two components and no other: whatever else an entity carries
 * (health, damage) is invisible to it. Inside a layer, entities are drawn by
 * ascending entity index, so the same registry always gives the same
 * sequence.
 */
class RenderSystem {
 public:
  /**
   * @brief Draws one frame, from the background layer to the interface layer.
   */
  void render(engine::ComponentRegistry& components, IDrawSurface& surface);

 private:
  struct DrawCall {
    std::uint32_t entityIndex;
    game::Position position;
    const std::string* assetId;
  };

  std::array<std::vector<DrawCall>, LAYER_COUNT> drawCallsByLayer_;
};

}  // namespace rtype::client
