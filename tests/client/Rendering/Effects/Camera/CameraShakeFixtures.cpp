#include "CameraShakeFixtures.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstdlib>
#include "CameraShake.hpp"
#include "CameraShakeTestConstants.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::CameraShake;

ShakeOverASecond shakeKeeping(float trauma) {
  CameraShake shake;
  ShakeOverASecond result;
  for (int frame = 0; frame < FRAMES_IN_A_SECOND; ++frame) {
    shake.addTrauma(trauma - shake.trauma());
    shake.advance(ONE_FRAME);
    const sf::Vector2i offset = shake.offset();
    result.largest =
        std::max({result.largest, std::abs(offset.x), std::abs(offset.y)});
    result.moved = result.moved || offset != sf::Vector2i();
  }
  return result;
}
