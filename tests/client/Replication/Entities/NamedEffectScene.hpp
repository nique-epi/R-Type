#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include "ComponentRegistry.hpp"
#include "EffectPlayer.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityRegistry.hpp"
#include "FlashPreference.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client::testing {

/** @brief A body of the sheet EFFECT_BODY_SHEET at EFFECT_BODY_POSITION,
 * whose appearance names the effect SPARK, the spawner of the effects, and
 * the player of the named effects. */
class NamedEffectScene {
 public:
  NamedEffectScene();

  /** @brief Plays the effect @p effect on the body @p bodyId, as an
   * EffectPlayed naming them does. */
  void play(std::uint32_t bodyId, std::string_view effect);
  /** @brief Gives the spawner @p preference for the flashes. */
  void preferFlashes(FlashPreference preference);
  /** @returns How many animated sprites stand in the Effects layer. */
  [[nodiscard]] std::size_t effectCount();
  /** @returns The frame the only effect shows, nothing without one. */
  [[nodiscard]] std::optional<game::SpriteFrame> effectFrame();
  /** @returns Whether the only effect follows the body, centered on it. */
  [[nodiscard]] bool effectFollowsTheBody();
  /** @returns The animation of the only effect; the test fails without one. */
  [[nodiscard]] SpriteAnimation effectAnimation();

 private:
  EntityAppearance appearance_;
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EffectSpawner effects_;
  EffectPlayer player_;
  engine::Entity body_{};
};

}  // namespace rtype::client::testing
