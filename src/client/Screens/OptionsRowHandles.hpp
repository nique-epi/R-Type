#pragma once

#include <array>
#include "BoundAction.hpp"

namespace rtype::client {

class Dropdown;
class KeySlot;

/** @brief The rows of the options whose state the descriptions follow, owned
 * by the menus that hold them. */
struct OptionsRowHandles {
  Dropdown* displayModes = nullptr;
  Dropdown* windowSizes = nullptr;
  std::array<KeySlot*, BOUND_ACTION_COUNT> keySlots{};
};

}  // namespace rtype::client
