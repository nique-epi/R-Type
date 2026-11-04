#pragma once

#include <functional>
#include <memory>
#include <optional>
#include "Button.hpp"
#include "RowControl.hpp"
#include "TextField.hpp"

namespace rtype::client {

/** @brief A bar MenuBuilder was asked for, not placed yet: a button, or a field
 * or a row when it holds one. */
struct PendingButton {
  Button::Text text;
  std::function<void()> action;
  std::optional<TextField> field;
  std::unique_ptr<RowControl> control;
};

}  // namespace rtype::client
