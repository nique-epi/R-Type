#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "IDrawSurface.hpp"
#include "Layer.hpp"
#include "Position.hpp"
#include "RenderSystem.hpp"
#include "Sprite.hpp"
#include "Velocity.hpp"

using rtype::client::IDrawSurface;
using rtype::client::RenderSystem;
using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::CollisionBox;
using rtype::game::Layer;
using rtype::game::Position;
using rtype::game::Sprite;
using rtype::game::Velocity;

namespace {

struct DrawnImage {
  std::string assetId;
  Position center;
};

class RecordingSurface : public IDrawSurface {
 public:
  void draw(std::string_view assetId, Position center) override {
    drawn_.push_back({.assetId = std::string(assetId), .center = center});
  }

  [[nodiscard]] const std::vector<DrawnImage>& drawn() const { return drawn_; }

  [[nodiscard]] std::vector<std::string> assetIds() const {
    std::vector<std::string> ids;
    ids.reserve(drawn_.size());
    for (const DrawnImage& image : drawn_) {
      ids.push_back(image.assetId);
    }
    return ids;
  }

  void forget() { drawn_.clear(); }

 private:
  std::vector<DrawnImage> drawn_;
};

constexpr float SPAWN_X = 1.0F;
constexpr float SPAWN_Y = 2.0F;
constexpr float SHIP_X = 40.0F;
constexpr float SHIP_Y = 25.0F;
constexpr float SPEED = 5.0F;
constexpr float BOX_SIZE = 9.0F;

class Scene {
 public:
  Entity spawn(const std::string& assetId, Layer layer) {
    const Entity entity = entities_.create();
    components_.add(entity, Position{.x = SPAWN_X, .y = SPAWN_Y});
    components_.add(entity, Sprite{.assetId = assetId, .layer = layer});
    return entity;
  }

  void render() { system_.render(components_, surface_); }

  [[nodiscard]] ComponentRegistry& components() { return components_; }
  [[nodiscard]] RecordingSurface& surface() { return surface_; }
  [[nodiscard]] EntityRegistry& entities() { return entities_; }

 private:
  EntityRegistry entities_;
  ComponentRegistry components_{entities_};
  RenderSystem system_;
  RecordingSurface surface_;
};

std::vector<std::string> allLayersInOrder() {
  return {"background", "ship", "explosion", "score"};
}

}  // namespace

/**
 * Given one entity in each of the four layers, created from the front layer to
 * the back layer
 * When a frame is rendered
 * Then the images are drawn from the background to the interface
 */
TEST(RenderSystem, DrawsLayersFromBackgroundToInterface) {
  Scene scene;
  scene.spawn("score", Layer::Interface);
  scene.spawn("explosion", Layer::Effects);
  scene.spawn("ship", Layer::Entities);
  scene.spawn("background", Layer::Background);

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(), allLayersInOrder());
}

/**
 * Given two entities in the same layer
 * When a frame is rendered
 * Then the one with the lower entity index is drawn first
 */
TEST(RenderSystem, DrawsEntitiesOfOneLayerByAscendingIndex) {
  Scene scene;
  const Entity first = scene.spawn("first", Layer::Entities);
  const Entity second = scene.spawn("second", Layer::Entities);
  ASSERT_LT(first.index, second.index);

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(),
            (std::vector<std::string>{"first", "second"}));
}

/**
 * Given an entity at a known position
 * When a frame is rendered
 * Then the image is drawn centered on that position
 */
TEST(RenderSystem, DrawsAtThePositionOfTheEntity) {
  Scene scene;
  const Entity entity = scene.spawn("ship", Layer::Entities);
  scene.components().get<Position>(entity)->x = SHIP_X;
  scene.components().get<Position>(entity)->y = SHIP_Y;

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_FLOAT_EQ(scene.surface().drawn()[0].center.x, SHIP_X);
  EXPECT_FLOAT_EQ(scene.surface().drawn()[0].center.y, SHIP_Y);
}

/**
 * Given an entity with a Position and no Sprite, and an entity with a Sprite
 * and no Position
 * When a frame is rendered
 * Then neither is drawn
 */
TEST(RenderSystem, SkipsEntitiesMissingPositionOrSprite) {
  Scene scene;
  const Entity withoutSprite = scene.entities().create();
  scene.components().add(withoutSprite, Position{.x = 0.0F, .y = 0.0F});
  const Entity withoutPosition = scene.entities().create();
  scene.components().add(withoutPosition,
                         Sprite{.assetId = "ghost", .layer = Layer::Entities});

  scene.render();

  EXPECT_TRUE(scene.surface().drawn().empty());
}

/**
 * Given an entity that also carries gameplay components
 * When a frame is rendered
 * Then it is drawn exactly like an entity without them
 */
TEST(RenderSystem, GameplayComponentsDoNotChangeWhatIsDrawn) {
  Scene scene;
  const Entity armed = scene.spawn("armed", Layer::Entities);
  scene.components().add(armed, Velocity{.x = SPEED, .y = SPEED});
  scene.components().add(armed,
                         CollisionBox{.width = BOX_SIZE, .height = BOX_SIZE});
  scene.spawn("plain", Layer::Entities);

  scene.render();

  const std::vector<DrawnImage>& drawn = scene.surface().drawn();
  ASSERT_EQ(drawn.size(), 2U);
  EXPECT_FLOAT_EQ(drawn[0].center.x, drawn[1].center.x);
  EXPECT_FLOAT_EQ(drawn[0].center.y, drawn[1].center.y);
}

/**
 * Given an entity that was destroyed
 * When a frame is rendered
 * Then it is not drawn
 */
TEST(RenderSystem, DoesNotDrawDestroyedEntities) {
  Scene scene;
  const Entity gone = scene.spawn("gone", Layer::Entities);
  scene.spawn("kept", Layer::Entities);
  scene.components().destroy(gone);

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(), (std::vector<std::string>{"kept"}));
}

/**
 * Given a registry holding no entity
 * When a frame is rendered
 * Then nothing is drawn
 */
TEST(RenderSystem, DrawsNothingForAnEmptyRegistry) {
  Scene scene;

  scene.render();

  EXPECT_TRUE(scene.surface().drawn().empty());
}

/**
 * Given a system that already rendered a frame
 * When a second frame is rendered with the same entities
 * Then the second frame draws the same sequence, with nothing left over from
 * the first
 */
TEST(RenderSystem, EachFrameDrawsTheSameSequence) {
  Scene scene;
  scene.spawn("ship", Layer::Entities);
  scene.spawn("background", Layer::Background);
  scene.render();
  const std::vector<std::string> firstFrame = scene.surface().assetIds();
  scene.surface().forget();

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(), firstFrame);
}
