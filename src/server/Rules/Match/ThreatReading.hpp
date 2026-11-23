#pragma once

#include "IThreatLevel.hpp"

namespace rtype::server {

/** @returns The threat of @p threat in percent of the most; none for a game
 * without the module threat, whose @p threat is nullptr. */
[[nodiscard]] int threatPercentOf(const IThreatLevel* threat);

}  // namespace rtype::server
