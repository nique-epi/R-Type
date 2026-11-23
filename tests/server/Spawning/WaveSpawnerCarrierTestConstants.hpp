#pragma once

#include <cstddef>

namespace rtype::server::testing {

/** @brief Where a crystal carrier enters, from the top: a quarter of the way
 * down, above a look item. */
constexpr float QUARTER_OF_THE_HEIGHT = 0.25F;

/** @brief The carriers a plan that brings one every other wave sends while
 * the waves 2 to 5 begin (see TICKS_OF_FOUR_WAVES): with the second and the
 * fourth. */
constexpr std::size_t CARRIERS_OF_FOUR_WAVES = 2;

}  // namespace rtype::server::testing
