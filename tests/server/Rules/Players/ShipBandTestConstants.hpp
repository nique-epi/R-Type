#pragma once

#include "PlayBand.hpp"

namespace rtype::server::testing {

/** @brief The band of the Pit, its ceiling on row 192 and its floor on row
 * 848. */
constexpr game::PlayBand PIT_BAND{.top = 192.0F, .bottom = 848.0F};

}  // namespace rtype::server::testing
