#pragma once

#include "EffectPlayed.hpp"
#include "EffectTools.hpp"

namespace rtype::client {

/**
 * @brief Plays the effects the server names by an EffectPlayed: the effect of
 * `[appearance.effects]` of the sheet of the entity, once, centered on the
 * entity and moved by the anchor of the effect, in the Effects layer, without
 * flash when the player reduces the flashes.
 *
 * A message naming an entity the client does not have, or an effect its sheet
 * does not name, plays nothing: the content of a server may name an effect the
 * pack of the client draws no frame for.
 */
class EffectPlayer {
 public:
  /** @param tools The registries and the flash gate of the effects, which
   * must outlive the player. */
  explicit EffectPlayer(EffectTools tools);

  void apply(const network::EffectPlayed& played);

 private:
  EffectTools tools_;
};

}  // namespace rtype::client
