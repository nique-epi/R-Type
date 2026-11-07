#pragma once

#include <string>
#include "EntityType.hpp"

namespace rtype::game {

/**
 * @brief What a client knows of an entity type it draws: the name of its
 * sheet and its side. Its number is its place in the list
 * numberedEntityTypesOf() gives, from 1.
 */
struct NumberedEntityType {
  std::string name;
  EntityType side{};
};

}  // namespace rtype::game
