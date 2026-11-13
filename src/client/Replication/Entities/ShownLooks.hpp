#pragma once

#include <cstdint>
#include "Animation.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"

namespace rtype::client {

/**
 * @returns The loop @p entity rests on between its actions: the look its
 * game::ShownLook names when its sheet has it, otherwise its enraged idle once
 * it is Enraged, otherwise the idle of @p appearance, its sheet. A look named
 * by the server wins over the rage, which the sheet only draws by default.
 */
[[nodiscard]] Animation restingAnimationOf(
    engine::ComponentRegistry& components, engine::Entity entity,
    const EntityAppearance& appearance);

/**
 * @brief Makes @p entity show the look numbered @p look, as a WorldState
 * lists it: records it in its game::ShownLook, then gives it its new resting
 * loop in place of the one it was resting on, or after the action it plays
 * when that action goes back to its idle. Does nothing when the entity already
 * shows that look, or has no appearance or no animation; a number the sheet
 * does not have shows its idle (lookAnimationOf()).
 */
void showLook(engine::ComponentRegistry& components, engine::Entity entity,
              std::uint8_t look);

}  // namespace rtype::client
