#pragma once

namespace rtype::client {

/** @returns A value from 0 to 1 that depends on @p seed only: the noise the
 * mockup draws the grains of a charge and the speed lines from (see
 * INTRO_HASH_FREQUENCY). */
[[nodiscard]] float introHash(double seed);

}  // namespace rtype::client
