#pragma once

#include <functional>
#include "CasinoBar.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChip.hpp"

namespace rtype::client {

/** @returns The chip of @p bar, which runs @p action and plays its click
 * through @p sounds, which must outlive it. */
[[nodiscard]] KeyChip chipOf(const CasinoBar& bar, std::function<void()> action,
                             ISoundPlayer& sounds);

}  // namespace rtype::client
