#pragma once

#include <cstdint>
#include "ParticleContent.hpp"

namespace rtype::client {

/** @brief What a layer of particles is built from: what its file says of
 * it, the width after which its layer repeats, 0 for never, and the seed of
 * its random draws. */
struct ParticleLayerParts {
  ParticleContent description;
  float repeat{};
  std::uint32_t seed{};
};

}  // namespace rtype::client
