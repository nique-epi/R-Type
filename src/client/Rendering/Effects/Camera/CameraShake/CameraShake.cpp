#include "CameraShake.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>
#include "CameraShakeConstants.hpp"
#include "RenderingConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns A value between -1 and 1 for one step of the noise of an axis:
 * always the same for the same step and seed. */
float latticeValue(std::int64_t step, std::uint32_t seed) {
  std::uint32_t mixed =
      (static_cast<std::uint32_t>(step) * NOISE_STEP_MULTIPLIER) ^
      (seed * NOISE_SEED_MULTIPLIER);
  mixed ^= mixed >> NOISE_SHIFT;
  mixed *= NOISE_FINAL_MULTIPLIER;
  mixed ^= mixed >> NOISE_SHIFT;
  const float share =
      static_cast<float>(mixed) /
      static_cast<float>(std::numeric_limits<std::uint32_t>::max());
  return (share - HALF) / HALF;
}

/** @brief The two axes the camera shakes along, each with its own noise. */
enum class ShakeAxis : std::uint8_t { Across, Down };

/** @returns The seed of the noise of an axis. */
std::uint32_t seedOf(ShakeAxis axis) {
  return axis == ShakeAxis::Across ? SHAKE_SEED_X : SHAKE_SEED_Y;
}

/** @returns The noise of an axis at a time counted in steps: the values of
 * the two steps around it, blended by a cosine, so it never jumps. */
float smoothNoise(double steps, ShakeAxis axis) {
  const std::uint32_t seed = seedOf(axis);
  const double whole = std::floor(steps);
  const auto fraction = static_cast<float>(steps - whole);
  const auto step = static_cast<std::int64_t>(whole);
  const float blend =
      (1.0F - std::cos(fraction * std::numbers::pi_v<float>)) * HALF;
  const float from = latticeValue(step, seed);
  return from + ((latticeValue(step + 1, seed) - from) * blend);
}

}  // namespace

void CameraShake::addTrauma(float amount) {
  trauma_ = std::clamp(trauma_ + std::max(amount, 0.0F), 0.0F, 1.0F);
}

void CameraShake::advance(engine::Duration elapsed) {
  const float seconds = std::chrono::duration<float>(elapsed).count();
  trauma_ = std::max(trauma_ - (TRAUMA_DECAY_PER_SECOND * seconds), 0.0F);
  noiseTime_ += elapsed;
}

sf::Vector2i CameraShake::offset() const {
  const float amplitude =
      SHAKE_MAXIMUM_OFFSET * std::pow(trauma_, SHAKE_EXPONENT);
  const double steps = std::chrono::duration<double>(noiseTime_).count() *
                       SHAKE_NOISE_STEPS_PER_SECOND;
  return {static_cast<int>(
              std::lround(amplitude * smoothNoise(steps, ShakeAxis::Across))),
          static_cast<int>(
              std::lround(amplitude * smoothNoise(steps, ShakeAxis::Down)))};
}

float CameraShake::trauma() const { return trauma_; }

}  // namespace rtype::client
