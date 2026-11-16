#pragma once

#include <functional>

namespace rtype::client {

/** @brief What the bars of the table do: CLEAR, REBET and BACK. */
struct RouletteBarActions {
  std::function<void()> clear;
  std::function<void()> rebet;
  std::function<void()> back;
};

}  // namespace rtype::client
