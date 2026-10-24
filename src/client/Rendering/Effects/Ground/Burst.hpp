#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The shape of a burst of particles: how many, how fast they leave,
 * in logical units per second, how long they live on average and by which
 * share of that it varies, how wide the cone they leave in, in radians, and
 * whether they shrink. Where it bursts and where it aims are given then.
 */
struct Burst {
  std::size_t count{};
  float slowest{};
  float fastest{};
  engine::Duration life{};
  float lifeSpread{};
  float cone{};
  bool shrinks = false;
};

}  // namespace rtype::client
