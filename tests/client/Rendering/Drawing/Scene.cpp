#include "Scene.hpp"
#include <string>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Layer.hpp"
#include "Position.hpp"
#include "RecordingDrawSurface.hpp"
#include "RenderSystemTestConstants.hpp"
#include "Sprite.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::Layer;
using rtype::game::Position;
using rtype::game::Sprite;

Entity Scene::spawn(const std::string& assetId, Layer layer) {
  const Entity entity = entities_.create();
  addPositionAndSprite(entity, assetId, layer);
  return entity;
}

void Scene::addPositionAndSprite(Entity entity, const std::string& assetId,
                                 Layer layer) {
  components_.add(entity, Position{.x = SPAWN_X, .y = SPAWN_Y});
  components_.add(
      entity, Sprite{.assetId = assetId, .frame = WHOLE_IMAGE, .layer = layer});
}

void Scene::render() { system_.render(components_, surface_); }

ComponentRegistry& Scene::components() { return components_; }

RecordingDrawSurface& Scene::surface() { return surface_; }

EntityRegistry& Scene::entities() { return entities_; }
