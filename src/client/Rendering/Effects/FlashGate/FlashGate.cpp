#include "FlashGate.hpp"
#include <SFML/Graphics/Color.hpp>
#include <span>
#include "CameraShake.hpp"
#include "ColorRamp.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "FlashLoan.hpp"
#include "FlashPreference.hpp"
#include "HitFlash.hpp"
#include "Nudge.hpp"
#include "SpriteTone.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void FlashGate::advance(engine::Duration elapsed, FlashPreference preference) {
  budget_.advance(elapsed);
  preference_ = preference;
}

bool FlashGate::flashes() const { return preference_ == FlashPreference::Full; }

bool FlashGate::mayFlash(FlashLoan loan) {
  return flashes() && budget_.reserve(loan);
}

HitFlash FlashGate::hitFlashFor(FlashLoan loan) {
  return HitFlash{
      .tone = mayFlash(loan) ? SpriteTone::Flash : SpriteTone::Shade,
      .remaining = loan.remaining};
}

std::span<const sf::Color> FlashGate::rampOf(
    std::span<const sf::Color> ramp) const {
  return rampFor(ramp, preference_);
}

void FlashGate::nudge(engine::ComponentRegistry& components,
                      engine::Entity entity, const Nudge& push) const {
  if (flashes()) {
    components.add(entity, push);
  }
}

void FlashGate::shake(CameraShake& camera, float trauma) const {
  if (flashes()) {
    camera.addTrauma(trauma);
  }
}

}  // namespace rtype::client
