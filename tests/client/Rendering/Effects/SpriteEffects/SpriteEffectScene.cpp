#include "SpriteEffectScene.hpp"
#include <string>
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Layer.hpp"
#include "Popup.hpp"
#include "Position.hpp"
#include "SpriteEffectSystemTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::Caption;
using rtype::client::Popup;
using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::Layer;
using rtype::game::Position;

SpriteEffectScene::SpriteEffectScene() : components_(entities_) {}

Entity SpriteEffectScene::add(Position position) {
  const Entity entity = entities_.create();
  components_.add(entity, position);
  return entity;
}

Entity SpriteEffectScene::addFigure() {
  const Entity figure = add(START);
  components_.add(figure, Caption{.text = std::string(GAIN_FIGURE_TEXT),
                                  .style = {},
                                  .layer = Layer::Popups,
                                  .visible = true});
  components_.add(figure, Popup{.originY = START.y, .age = Duration::zero()});
  return figure;
}

Entity SpriteEffectScene::addSteadyFigure() {
  const Entity figure = addFigure();
  components_.get<Popup>(figure)->steady = true;
  return figure;
}

void SpriteEffectScene::run(Duration elapsed) {
  system_.update(components_, elapsed);
}

ComponentRegistry& SpriteEffectScene::components() { return components_; }

bool SpriteEffectScene::isAlive(Entity entity) const {
  return entities_.isAlive(entity);
}
