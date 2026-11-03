#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <span>
#include <string_view>

/**
 * @brief Hands @p arguments to @p options one option after the other, through
 * its readOption(), as a program that has no option of its own would. Fails
 * the running test at the first argument that is none of these options.
 */
template <typename Options>
void readEveryOption(Options& options,
                     std::span<const std::string_view> arguments) {
  std::size_t index = 0;
  while (index < arguments.size()) {
    const std::size_t taken = options.readOption(arguments.subspan(index));
    if (taken == 0) {
      ADD_FAILURE() << arguments[index];
      return;
    }
    index += taken;
  }
}
