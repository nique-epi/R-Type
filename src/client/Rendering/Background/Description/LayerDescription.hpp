#pragma once

#include <string>
#include "LayerContent.hpp"
#include "LayerMotion.hpp"

namespace rtype::client {

/** @brief One layer of a background, as its file describes it: a depth that
 * moves at its own speed and shows one content. */
struct LayerDescription {
  /** How the journal and the errors name it. */
  std::string name;
  LayerMotion motion;
  /** Whether the background arrives through this layer: the layers before it
   * show only right of where it starts, and the background that leaves stays
   * behind until it has left the screen. */
  bool entrance{};
  LayerContent content;
};

}  // namespace rtype::client
