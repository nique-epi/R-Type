#include "PlayerInputPacer.hpp"
#include "ConnectionConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

bool PlayerInputPacer::advance(engine::Duration elapsed) {
  sinceLastInput_ += elapsed;
  if (sinceLastInput_ < PLAYER_INPUT_INTERVAL) {
    return false;
  }
  sinceLastInput_ -= PLAYER_INPUT_INTERVAL;
  if (sinceLastInput_ >= PLAYER_INPUT_INTERVAL) {
    sinceLastInput_ = engine::Duration::zero();
  }
  return true;
}

}  // namespace rtype::client
