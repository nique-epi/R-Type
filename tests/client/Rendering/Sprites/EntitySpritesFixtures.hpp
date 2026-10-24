#pragma once

#include <vector>
#include "EntityType.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"

/** @returns The frame of the sprite of @p type, or NO_FRAME when it has no
 * sprite. */
rtype::game::SpriteFrame frameOf(rtype::game::EntityType type);

/** @returns Every frame of the animations of every entity type. */
std::vector<rtype::game::SpriteFrame> everyAnimationFrame();

/** @returns The frame an animation shows where it starts. */
rtype::game::SpriteFrame startFrameOf(
    const rtype::client::SpriteAnimation& animation);
