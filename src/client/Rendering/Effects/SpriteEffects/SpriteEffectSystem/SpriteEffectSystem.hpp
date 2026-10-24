#pragma once

#include <vector>
#include "Entity.hpp"
#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief Moves the effects drawn on sprites forward: nudges, lifetimes,
 * anchors, figures of a gain, reveals and invulnerabilities.
 *
 * A nudge or a reveal whose time is spent during a step is removed at the end
 * of that step; an entity whose lifetime is spent, or whose figure has
 * finished, is destroyed, which waits until the walk over the components
 * ends. The picture drawn after the step no longer shows them.
 */
class SpriteEffectSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  void advanceNudges(engine::ComponentRegistry& components,
                     engine::Duration elapsed);
  static void advanceLifetimes(engine::ComponentRegistry& components,
                               engine::Duration elapsed);
  static void followAnchors(engine::ComponentRegistry& components);
  static void advancePopups(engine::ComponentRegistry& components,
                            engine::Duration elapsed);
  void advanceReveals(engine::ComponentRegistry& components,
                      engine::Duration elapsed);
  static void advanceInvulnerabilities(engine::ComponentRegistry& components,
                                       engine::Duration elapsed);

  /** @brief Removes the nudges and the reveals spent during the step. */
  void removeSpent(engine::ComponentRegistry& components);

  std::vector<engine::Entity> spentNudges_;
  std::vector<engine::Entity> spentReveals_;
};

}  // namespace rtype::client
