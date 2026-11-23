#pragma once

#include <cstddef>
#include "Gun.hpp"
#include "GunSheet.hpp"
#include "ShotPattern.hpp"

namespace rtype::game::testing {

/** @returns The gun of a sheet that fires @p pattern every @p intervalTicks,
 * ready at once. */
[[nodiscard]] GunSheet gunSheetFiring(const ShotPattern& pattern,
                                      std::size_t intervalTicks);

/** @returns A gun held by a ship that fires @p pattern every
 * @p intervalTicks. */
[[nodiscard]] Gun gunFiring(const ShotPattern& pattern,
                            std::size_t intervalTicks);

}  // namespace rtype::game::testing
