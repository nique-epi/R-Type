#pragma once

#include <memory>
#include "ComponentRegistry.hpp"
#include "EntityRegistry.hpp"
#include "IEntityLayer.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"

namespace rtype::client {

/**
 * @brief The game in progress: its entities, drawn over the scrolling
 * background, which the frame loop draws under every screen.
 *
 * The screen owns the entities of the game, which appear and disappear with
 * it. Since draw() is const, each update repaints them on the entity layer and
 * each draw only lays that layer; the layer is painted once at construction,
 * so the first frame already shows them. Until the client creates the entities
 * the server announces, the screen holds the provisional sprite showcase.
 *
 * Releasing OPTIONS_KEY puts the options screen over the game, which goes on
 * running and stays drawn behind it. The release, not the press: the system
 * repeats the press of a held key, which would open and close the options
 * again and again.
 */
class GameScreen : public IScreen {
 public:
  /**
   * @param navigator Screens the game lives among, which must outlive it;
   * the options are pushed over the game when OPTIONS_KEY is released.
   * @param entityLayer Picture the entities are painted on, never null.
   */
  GameScreen(IScreenNavigator& navigator,
             std::unique_ptr<IEntityLayer> entityLayer);

  void handleEvent(const sf::Event& event) override;
  void update() override;
  void draw(sf::RenderTarget& target) const override;

 private:
  IScreenNavigator* navigator_;
  std::unique_ptr<IEntityLayer> entityLayer_;
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
};

}  // namespace rtype::client
