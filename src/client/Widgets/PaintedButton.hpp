#pragma once

#include <cstddef>
#include <functional>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief A button of a menu being painted: its order, the state it shows,
 * and the canvas it is painted on. */
struct PaintedButton {
  std::size_t order{};
  BarState state{};
  std::reference_wrapper<IInterfaceCanvas> canvas;
};

}  // namespace rtype::client
