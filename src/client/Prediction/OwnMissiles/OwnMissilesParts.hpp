#pragma once

#include <functional>
#include "ComponentRegistry.hpp"
#include "EntityFeedback.hpp"
#include "EntityRegistry.hpp"
#include "IEntityTypeSource.hpp"

namespace rtype::client {

/**
 * @brief What OwnMissiles is built from: the registries of the game screen it
 * draws its missiles in, the effects and sounds of their shots and impacts,
 * and the types of the session, which give the gun of the ships and the look
 * of their missile.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct OwnMissilesParts {
  std::reference_wrapper<engine::EntityRegistry> entities;
  std::reference_wrapper<engine::ComponentRegistry> components;
  EntityFeedback feedback;
  std::reference_wrapper<const IEntityTypeSource> types;
};

}  // namespace rtype::client
