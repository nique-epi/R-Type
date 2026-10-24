#pragma once

#include <string_view>

namespace rtype::client {

/** @brief What the intro plays at a moment of the scene: a sound of
 * SoundConstants.hpp, none when empty, and a trauma added to the shake of
 * the camera, 0 for none. */
struct IntroCue {
  float at{};
  std::string_view sound;
  float trauma{};
};

}  // namespace rtype::client
