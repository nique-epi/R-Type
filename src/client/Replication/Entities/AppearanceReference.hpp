#pragma once

namespace rtype::client {

struct EntityAppearance;

/**
 * @brief Component of an entity created from an entity sheet: the appearance
 * of its type, which EntityAppearances owns for as long as the client runs, so
 * the animations of its attacks can be found when the server announces one.
 */
struct AppearanceReference {
  const EntityAppearance* appearance;
};

}  // namespace rtype::client
