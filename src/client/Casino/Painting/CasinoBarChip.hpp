#pragma once

#include "CasinoBar.hpp"
#include "CasinoChipWiring.hpp"
#include "KeyChip.hpp"

namespace rtype::client {

/** @returns The chip of @p bar, labelled in the language of @p wiring, which
 * runs its action and plays its click through its sounds. */
[[nodiscard]] KeyChip chipOf(const CasinoBar& bar, CasinoChipWiring wiring);

}  // namespace rtype::client
