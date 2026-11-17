#pragma once

#include <string>

namespace rtype::client {

/** @brief A layer that shows sprites placed one by one: each entity of one
 * entity layer of an LDtk project is a sprite, the rectangle its tile names
 * cut from the tileset, where the entity lies. The project and the tileset
 * are asset ids. */
struct PropContent {
  std::string project;
  std::string layer;
  std::string tileset;
};

}  // namespace rtype::client
