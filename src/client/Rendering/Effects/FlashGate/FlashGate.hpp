#pragma once

#include <SFML/Graphics/Color.hpp>
#include <span>
#include "Entity.hpp"
#include "FlashBudget.hpp"
#include "FlashLoan.hpp"
#include "FlashPreference.hpp"
#include "HitFlash.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

class CameraShake;
struct Nudge;

/**
 * @brief What the flash preference of the player lets the effects do, and the
 * flash budget they share.
 *
 * With FlashPreference::Full, an effect flashes when the FlashBudget lends
 * the pixels of its flash, and nudges and shakes play. With
 * FlashPreference::Reduced, nothing flashes, nudges or shakes, and a ramp of
 * colors loses its first, hottest color. The preference is the one the last
 * advance() gave, FlashPreference::Full before any: an effect reads it when
 * it starts, so an effect already playing keeps its version. An
 * invulnerability is the exception: it lasts as long as the server holds it,
 * so PilotEffects reads the preference for it at every advance.
 */
class FlashGate {
 public:
  /** @brief Moves the flash budget on, and sets the preference the effects
   * that start from now on read. */
  void advance(engine::Duration elapsed, FlashPreference preference);

  /** @returns Whether the preference allows flashes, nudges and shakes. */
  [[nodiscard]] bool flashes() const;

  /** @returns Whether an effect flashes: flashes are allowed and the budget
   * lends the pixels of the loan, which it then holds. */
  [[nodiscard]] bool mayFlash(FlashLoan loan);

  /** @returns A hit flash lasting as long as the loan: white when it may
   * flash, else shaded. */
  [[nodiscard]] HitFlash hitFlashFor(FlashLoan loan);

  /** @returns The ramp, or the ramp without its first color when flashes
   * are reduced. */
  [[nodiscard]] std::span<const sf::Color> rampOf(
      std::span<const sf::Color> ramp) const;

  /** @brief Draws an entity a few pixels away for a moment (see Nudge),
   * unless flashes are reduced. */
  void nudge(engine::ComponentRegistry& components, engine::Entity entity,
             const Nudge& push) const;

  /** @brief Adds trauma to the camera shake, unless flashes are reduced. */
  void shake(CameraShake& camera, float trauma) const;

 private:
  FlashPreference preference_ = FlashPreference::Full;
  FlashBudget budget_;
};

}  // namespace rtype::client
