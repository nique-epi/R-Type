#include "Squadron.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string_view>
#include <vector>
#include "Animation.hpp"
#include "BackgroundConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColors.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "RandomRange.hpp"
#include "RenderingConstants.hpp"
#include "SpriteFrame.hpp"
#include "SquadronConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The value rounded to the nearest whole number, halves rounded up,
 * as Math.round in the mockup. */
int roundHalfUp(float value) {
  return static_cast<int>(std::floor(value + HALF));
}

/** @returns The value rounded to the nearest multiple of the step. */
int stepped(float value, int step) {
  return roundHalfUp(value / static_cast<float>(step)) * step;
}

/** @returns How far a braking flight has gone, from 0 to 1, after a share of
 * its time from 0 to 1: 1 − (1 − share)³. */
float easeOut(float share) {
  const float left = 1.0F - share;
  return 1.0F - (left * left * left);
}

/** @returns 0 before 0, 1 after 1, and an S-shaped ramp between:
 * share² (3 − 2 share), written with 1 − share. */
float smoothStep(float share) {
  const float clamped = std::clamp(share, 0.0F, 1.0F);
  const float left = 1.0F - clamped;
  return clamped * clamped * (1.0F + left + left);
}

/**
 * @brief Paints the four corners of the box, the top ones that many rows
 * below its top edge and the bottom ones as far above its bottom edge, each
 * two strokes SQUADRON_CORNER_LENGTH long.
 */
void paintCorners(IInterfaceCanvas& canvas, sf::IntRect box, int rowsIn) {
  const int left = box.position.x;
  const int right = box.position.x + box.size.x - 1;
  const int upper = box.position.y + rowsIn;
  const int lower = box.position.y + std::max(rowsIn, box.size.y - 1 - rowsIn);
  const int farEnd = SQUADRON_CORNER_LENGTH - 1;
  const sf::Vector2i across(SQUADRON_CORNER_LENGTH, 1);
  const sf::Vector2i down(1, SQUADRON_CORNER_LENGTH);
  canvas.fill({{left, upper}, across}, EDGE_BLUE);
  canvas.fill({{left, upper}, down}, EDGE_BLUE);
  canvas.fill({{right - farEnd, upper}, across}, EDGE_BLUE);
  canvas.fill({{right, upper}, down}, EDGE_BLUE);
  canvas.fill({{left, lower}, across}, EDGE_BLUE);
  canvas.fill({{left, lower - farEnd}, down}, EDGE_BLUE);
  canvas.fill({{right - farEnd, lower}, across}, EDGE_BLUE);
  canvas.fill({{right, lower - farEnd}, down}, EDGE_BLUE);
}

/** @brief Paints the line growing from the middle of the box, or its corners
 * moving apart, or the open box. */
void paintFrame(IInterfaceCanvas& canvas, sf::IntRect box, float time) {
  const sf::Vector2i middle(sf::Vector2f(box.size) * HALF);
  if (time < SQUADRON_LINE_END) {
    const int half =
        stepped(static_cast<float>(middle.x) * time / SQUADRON_LINE_END,
                SQUADRON_OPENING_STEP);
    if (half > 0) {
      canvas.fill(
          {{box.position.x + middle.x - half, box.position.y + middle.y},
           {half + half, 1}},
          EDGE_BLUE);
    }
    return;
  }
  const float opened =
      std::min(1.0F, (time - SQUADRON_LINE_END) /
                         (SQUADRON_OPEN_END - SQUADRON_LINE_END));
  paintCorners(
      canvas, box,
      std::min(middle.y, stepped(static_cast<float>(middle.y) * (1.0F - opened),
                                 SQUADRON_OPENING_STEP)));
}

/** @brief Writes as many letters of SQUADRON as have appeared. */
void paintTitle(IInterfaceCanvas& canvas, sf::IntRect box, float time) {
  if (time < SQUADRON_TITLE_START) {
    return;
  }
  const auto letters = std::min(
      SQUADRON_TITLE.size(),
      static_cast<std::size_t>(
          std::floor((time - SQUADRON_TITLE_START) / SQUADRON_LETTER_TIME)) +
          1);
  canvas.write(SQUADRON_TITLE.substr(0, letters),
               box.position + SQUADRON_TITLE_POSITION, SQUADRON_TITLE_STYLE);
}

}  // namespace

Squadron::Squadron(MotionPreference motion, std::uint32_t seed)
    : motion_(motion),
      random_(seed),
      elapsed_(motion == MotionPreference::Reduced
                   ? std::chrono::duration_cast<engine::Duration>(
                         std::chrono::duration<float>(SQUADRON_DRIFT_START))
                   : engine::Duration::zero()) {}

void Squadron::advance(engine::Duration elapsed) {
  if (motion_ == MotionPreference::Reduced) {
    return;
  }
  const engine::Duration step =
      std::clamp(elapsed, engine::Duration::zero(), MAXIMUM_BACKGROUND_STEP);
  elapsed_ += step;
  const float now = seconds();
  if (now >= SQUADRON_FIRE_START && now >= nextShot_) {
    fire(now);
  }
  const float stepSeconds = std::chrono::duration<float>(step).count();
  for (sf::Vector2f& missile : missiles_) {
    missile.x += SQUADRON_MISSILE_SPEED * stepSeconds;
  }
  std::erase_if(missiles_, [](sf::Vector2f missile) {
    return missile.x >= static_cast<float>(SQUADRON_BOX_SIZE.x);
  });
}

void Squadron::paint(IInterfaceCanvas& canvas, sf::Vector2i topLeft) const {
  const sf::IntRect box(topLeft, SQUADRON_BOX_SIZE);
  const float time = seconds();
  paintFrame(canvas, box, time);
  paintTitle(canvas, box, time);
  const sf::IntRect missileFrame =
      rectangleOf(PLAYER_MISSILE_ANIMATION.frameAt(elapsed_).value_or(
          PLAYER_MISSILE_FRAME));
  for (const sf::Vector2f missile : missiles_) {
    canvas.drawImage(ENTITY_SHEET_ID, missileFrame,
                     box.position + sf::Vector2i(roundHalfUp(missile.x),
                                                 roundHalfUp(missile.y)),
                     NATIVE_SCALE, box);
  }
  const game::SpriteFrame shipFrame =
      PLAYER_SHIP_ANIMATION.frameAt(elapsed_).value_or(PLAYER_SHIP_FRAME);
  for (const ShipPlacement& placement : placements(time)) {
    canvas.drawImage(
        ENTITY_SHEET_ID,
        rectangleOf(inPlayerColorRow(shipFrame, placement.ship->color)),
        box.position + placement.topLeft, NATIVE_SCALE, box);
  }
}

float Squadron::seconds() const {
  return std::chrono::duration<float>(elapsed_).count();
}

std::vector<Squadron::ShipPlacement> Squadron::placements(float time) {
  const float drift =
      smoothStep((time - SQUADRON_DRIFT_START) / SQUADRON_DRIFT_RAMP);
  const float angle = FULL_TURN * time / SQUADRON_DRIFT_PERIOD;
  const sf::Vector2i driftOffset(
      roundHalfUp(SQUADRON_DRIFT_X * drift * std::sin(angle)),
      roundHalfUp(SQUADRON_DRIFT_Y * drift *
                  std::sin(SQUADRON_DRIFT_VERTICAL_LOOPS * angle)));
  std::vector<ShipPlacement> placed;
  placed.reserve(SQUADRON_SHIPS.size());
  for (const SquadronShip& ship : SQUADRON_SHIPS) {
    if (time < ship.arrival) {
      continue;
    }
    const float flight =
        std::min(1.0F, (time - ship.arrival) / SQUADRON_FLIGHT_TIME);
    const float progress = easeOut(flight);
    const int bob = roundHalfUp(
        SQUADRON_BOB * drift *
        std::sin((FULL_TURN * time / SQUADRON_BOB_PERIOD) + ship.bobPhase));
    const sf::Vector2i flown(
        roundHalfUp(SQUADRON_ENTRY_X +
                    ((ship.place.x - SQUADRON_ENTRY_X) * progress)),
        roundHalfUp(ship.place.y + (ship.entryDrop * (1.0F - progress))));
    placed.push_back({.ship = &ship,
                      .topLeft = flown + driftOffset + sf::Vector2i(0, bob),
                      .landed = flight >= 1.0F});
  }
  return placed;
}

void Squadron::fire(float time) {
  std::vector<ShipPlacement> ready = placements(time);
  std::erase_if(
      ready, [](const ShipPlacement& placement) { return !placement.landed; });
  if (!ready.empty()) {
    std::uniform_int_distribution<std::size_t> shooters(0, ready.size() - 1);
    const ShipPlacement& shooter = ready[shooters(random_)];
    missiles_.push_back(sf::Vector2f(shooter.topLeft) + SQUADRON_MUZZLE);
  }
  nextShot_ = time + randomBetween(random_, SQUADRON_SHORTEST_FIRE_GAP,
                                   SQUADRON_LONGEST_FIRE_GAP);
}

}  // namespace rtype::client
