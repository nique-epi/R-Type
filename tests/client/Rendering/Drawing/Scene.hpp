#pragma once

#include <string>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Layer.hpp"
#include "RecordingDrawSurface.hpp"
#include "RenderSystem.hpp"

/**
 * @brief Registries, the render system and a surface that records what it
 * draws: the entities a test spawns are drawn there.
 */
class Scene {
 public:
  /**
   * @brief Creates an entity at SPAWN_X, SPAWN_Y showing WHOLE_IMAGE of
   * @p assetId in @p layer.
   */
  rtype::engine::Entity spawn(const std::string& assetId,
                              rtype::game::Layer layer);

  /**
   * @brief Gives @p entity a position at SPAWN_X, SPAWN_Y and a sprite showing
   * WHOLE_IMAGE of @p assetId in @p layer.
   */
  void addPositionAndSprite(rtype::engine::Entity entity,
                            const std::string& assetId,
                            rtype::game::Layer layer);

  /** @brief Renders a frame on the surface. */
  void render();

  [[nodiscard]] rtype::engine::ComponentRegistry& components();
  [[nodiscard]] RecordingDrawSurface& surface();
  [[nodiscard]] rtype::engine::EntityRegistry& entities();

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_{entities_};
  rtype::client::RenderSystem system_;
  RecordingDrawSurface surface_;
};
