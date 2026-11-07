#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rtype::client::testing {

/**
 * @brief A Bydo flying along x at a constant speed whose states reach the
 * client every ticksBetweenStates ticks, except the lost ones, with the
 * arrival of some of them one tick late.
 */
struct InterpolationScenario {
  std::uint32_t ticksBetweenStates{};
  std::vector<std::size_t> lostStates;
};

/** @returns The states numbers 19, 39, 59 and so on up to the end of the
 * scenario: one state in twenty, 5 %. */
std::vector<std::size_t> everyTwentiethState();

/** @returns The same, plus two states lost in a row in the middle. */
std::vector<std::size_t> everyTwentiethStateAndTwoInARow();

/** @returns The largest distance along x between two frames of the Bydo as the
 * client shows it, in the frames after the clock settled, when it shows the
 * entity where the interpolation puts it. */
float largestStepInterpolated(const InterpolationScenario& scenario);

/** @returns The same when the client shows the entity where the newest state
 * received puts it, as before interpolation. */
float largestStepOfTheNewestState(const InterpolationScenario& scenario);

}  // namespace rtype::client::testing
