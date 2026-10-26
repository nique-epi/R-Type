#include "IntroTimeline.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include "CinemaView.hpp"
#include "Easing.hpp"
#include "InterfaceConstants.hpp"
#include "IntroFreeze.hpp"
#include "IntroTimelineConstants.hpp"
#include "MissionStartConstants.hpp"
#include "PixelRounding.hpp"
#include "RenderingConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The speed of the stars from the boost: up to the warp in
 * INTRO_WARP_RISE, held until INTRO_SCENE_WARP_END, then back to their own
 * speed, fast then slowly, when the title opens. */
float boostFactorAt(float scene) {
  if (scene < INTRO_SCENE_BOOST + INTRO_WARP_RISE) {
    return std::lerp(
        INTRO_CHARGE_SPEED, WARP_TOP_SPEED_FACTOR,
        progressThrough(scene, INTRO_SCENE_BOOST, INTRO_WARP_RISE));
  }
  if (scene < INTRO_SCENE_WARP_END) {
    return WARP_TOP_SPEED_FACTOR;
  }
  return std::lerp(
      WARP_TOP_SPEED_FACTOR, INTRO_OWN_SPEED,
      easeOutCubic(progressThrough(scene, INTRO_SCENE_WARP_END,
                                   INTRO_SCENE_TITLE - INTRO_SCENE_WARP_END)));
}

/** @returns The view zoomed INTRO_CLOSEUP_ZOOM times on @p focus, kept inside
 * the world. */
CinemaView zoomedOn(sf::Vector2f focus) {
  const sf::Vector2f shown = sf::Vector2f(INTERFACE_AREA.size) /
                             static_cast<float>(INTRO_CLOSEUP_ZOOM);
  const sf::Vector2f farthest = sf::Vector2f(INTERFACE_AREA.size) - shown;
  const sf::Vector2f topLeft(
      std::clamp(focus.x - (shown.x * HALF), 0.0F, farthest.x),
      std::clamp(focus.y - (shown.y * HALF), 0.0F, farthest.y));
  return CinemaView{.zoom = INTRO_CLOSEUP_ZOOM,
                    .topLeft = nearestPixel(topLeft)};
}

}  // namespace

float sceneTimeAt(float real, std::span<const IntroFreeze> freezes) {
  float scene = real;
  for (const IntroFreeze& freeze : freezes) {
    scene -= std::clamp(real - freeze.at, 0.0F, freeze.length);
  }
  return scene;
}

float skyFactorAt(float scene) {
  if (scene < INTRO_BEAT) {
    return INTRO_COLD_OPEN_SPEED;
  }
  if (scene < INTRO_SWARM_CUT) {
    return INTRO_CLOSEUP_SPEED;
  }
  if (scene < INTRO_SCENE_CHARGE) {
    return std::lerp(
        INTRO_SWARM_FIRST_SPEED, INTRO_SWARM_SPEED,
        progressThrough(scene, INTRO_SWARM_CUT, INTRO_SWARM_SLOWDOWN));
  }
  if (scene < INTRO_SCENE_BOOST) {
    return std::lerp(INTRO_SWARM_SPEED, INTRO_CHARGE_SPEED,
                     easeOutCubic(progressThrough(scene, INTRO_SCENE_CHARGE,
                                                  INTRO_CHARGE_SLOWDOWN)));
  }
  return boostFactorAt(scene);
}

CinemaView cinemaViewAt(float real) {
  if (real < INTRO_BEAT) {
    return zoomedOn(INTRO_COLD_OPEN_FOCUS);
  }
  if (real < INTRO_SWARM_CUT) {
    const auto pilot = static_cast<std::size_t>(real / INTRO_BEAT) - 1;
    return zoomedOn(INTRO_CLOSEUP_FOCUS +
                    sf::Vector2f(0.0F, INTRO_CLOSEUP_OFFSETS.at(pilot)));
  }
  return CinemaView{};
}

float debrisDriftAt(float scene) {
  const float since = scene - INTRO_SCENE_BOOST;
  if (since <= 0.0F) {
    return 0.0F;
  }
  const auto steps = static_cast<int>(std::ceil(since / INTRO_SKY_STEP));
  float way = 0.0F;
  for (int step = 0; step < steps; ++step) {
    const float from =
        INTRO_SCENE_BOOST + (static_cast<float>(step) * INTRO_SKY_STEP);
    way += skyFactorAt(from) * std::min(INTRO_SKY_STEP, scene - from);
  }
  return way * INTRO_DEBRIS_SPEED;
}

float closeupStartOf(std::size_t pilot) {
  return static_cast<float>(pilot + 1) * INTRO_BEAT;
}

float skipTargetAt(float now, std::optional<float> music) {
  if (!music.has_value()) {
    return std::max(now, INTRO_SLAM);
  }
  float sinceBeat = std::fmod(*music, INTRO_BEAT);
  if (sinceBeat < 0.0F) {
    sinceBeat += INTRO_BEAT;
  }
  const float untilBeat = sinceBeat > 0.0F ? INTRO_BEAT - sinceBeat : 0.0F;
  return std::max(now, INTRO_SLAM - untilBeat);
}

bool isOddStepAt(float time, float stepsPerSecond) {
  return static_cast<std::int64_t>(std::floor(time * stepsPerSecond)) % 2 != 0;
}

engine::Duration durationOf(float seconds) {
  return std::chrono::duration_cast<engine::Duration>(
      std::chrono::duration<float>(seconds));
}

float secondsOf(engine::Duration duration) {
  return std::chrono::duration<float>(duration).count();
}

}  // namespace rtype::client
