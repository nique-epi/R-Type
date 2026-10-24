#pragma once

#include <optional>
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "SheetEffect.hpp"

namespace rtype::client {

/**
 * @returns The entity of an effect of the sheet, in the Effects layer,
 * centered where the effect plays, whose animation plays once: the standard
 * version when the gate lets it flash (see FlashGate::mayFlash), else the
 * version without flash. The animation starts at minus the hold of the
 * effect, so its first frame stays that much longer. Nothing for an effect
 * without frame. It adds components, so it may not be called during a
 * forEach of the registry.
 */
std::optional<engine::Entity> spawnSheetEffect(const EffectTools& tools,
                                               const SheetEffect& effect);

}  // namespace rtype::client
