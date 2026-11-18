#pragma once

#include <functional>
#include "IParticleEmitter.hpp"

namespace rtype::client {

/**
 * @brief One layer of an emission group: where its particles are born, and
 * its chance among the layers of the group.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct EmissionGroupMember {
  std::reference_wrapper<IParticleEmitter> emitter;
  double weight{};
};

}  // namespace rtype::client
