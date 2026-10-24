#include "IntroShipPaths.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <optional>
#include <utility>
#include "BackgroundConstants.hpp"
#include "Easing.hpp"
#include "IntroCastConstants.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"
#include "RenderingConstants.hpp"
#include "SquadronConstants.hpp"
#include "SquadronShip.hpp"

namespace rtype::client {

namespace {

/** @returns When the ship of @p pilot starts entering the wide shot. */
float entryStartOf(std::size_t pilot) {
  return INTRO_SWARM_CUT + (static_cast<float>(pilot) * INTRO_ENTRY_STAGGER);
}

/** @returns The column of a ship entering from INTRO_ENTRY_X to its home,
 * overshooting it a little. */
float entryXAt(std::size_t pilot, float scene) {
  return std::lerp(INTRO_ENTRY_X, INTRO_HOMES.at(pilot).x,
                   easeOutBack(progressThrough(scene, entryStartOf(pilot),
                                               INTRO_ENTRY_FLIGHT)));
}

/** @returns How far the pink and green ships moved from their homes as they
 * swap: to the row of the other, bulging forward and back on the way;
 * nothing for the others. */
sf::Vector2f swapShiftAt(std::size_t pilot, float scene) {
  if (pilot != INTRO_PINK_PILOT && pilot != INTRO_GREEN_PILOT) {
    return {};
  }
  const bool pink = pilot == INTRO_PINK_PILOT;
  const std::size_t other = pink ? INTRO_GREEN_PILOT : INTRO_PINK_PILOT;
  const float swap =
      easeInOutCubic(progressThrough(scene, INTRO_SWAP, INTRO_SWAP_TIME));
  const float bulge =
      INTRO_SWAP_BULGE * std::sin(std::numbers::pi_v<float> * swap);
  return {pink ? bulge : -bulge,
          (INTRO_HOMES.at(other).y - INTRO_HOMES.at(pilot).y) * swap};
}

/** @returns The sway of the squadron, in whole pixels, fading as the charge
 * starts. */
float swayAt(float scene) {
  const float wave =
      std::sin(FULL_TURN * (scene - INTRO_SWARM_CUT) / INTRO_SWAY_PERIOD);
  const float kept =
      1.0F - progressThrough(scene, INTRO_SCENE_CHARGE, INTRO_SWAY_FADE);
  return std::round(INTRO_SWAY * wave * kept);
}

/** @returns The bob of a ship, one pixel up or down, out of step with the
 * others. */
float bobAt(std::size_t pilot, float scene) {
  return std::round(
      std::sin((FULL_TURN * scene / INTRO_BOB_PERIOD) +
               (static_cast<float>(pilot) * INTRO_BOB_PHASE_STEP)));
}

/** @returns How far back the charge pulls the squadron, with its tremble
 * just before the release and its recoil after. */
float chargeShiftAt(float scene) {
  float shift =
      -INTRO_PULL_BACK * easeOutCubic(progressThrough(scene, INTRO_SCENE_CHARGE,
                                                      INTRO_PULL_BACK_TIME));
  if (scene >= INTRO_SCENE_RELEASE - INTRO_TREMBLE_TIME &&
      scene < INTRO_SCENE_RELEASE) {
    shift += isOddStepAt(scene, INTRO_FLICKER_STEPS_PER_SECOND) ? 1.0F : -1.0F;
  }
  if (scene >= INTRO_SCENE_RELEASE) {
    shift -= INTRO_RECOIL *
             std::exp(-(scene - INTRO_SCENE_RELEASE) * INTRO_RECOIL_DECAY);
  }
  return shift;
}

/** @returns When the ship of @p pilot starts landing, one after the other
 * from the boost. */
float landingStartOf(std::size_t pilot) {
  return INTRO_SCENE_BOOST +
         (static_cast<float>(pilot) * INTRO_LANDING_STAGGER);
}

/** @returns How far the ship of @p pilot went of its way to its place in the
 * squadron of the title, from 0 to 1: fast first, then braking. */
float landingAt(std::size_t pilot, float scene) {
  return easeOutCubic(
      progressThrough(scene, landingStartOf(pilot), INTRO_LANDING_TIME));
}

/** @returns Where the ship of @p pilot sits in the squadron of the title, from
 * the top-left corner of its box; the corner itself for a pilot it has not. */
sf::Vector2f titlePlaceOf(std::size_t pilot) {
  for (const SquadronShip& ship : SQUADRON_SHIPS) {
    if (std::cmp_equal(ship.color, pilot)) {
      return ship.place;
    }
  }
  return {};
}

}  // namespace

std::optional<sf::Vector2f> closeupPlaceAt(std::size_t pilot, float scene) {
  const float start = closeupStartOf(pilot);
  if (scene < start || scene >= start + INTRO_BEAT) {
    return std::nullopt;
  }
  const float through = (scene - start) / INTRO_BEAT;
  const float fromMiddle = through - HALF;
  return sf::Vector2f(
      INTRO_CLOSEUP_FOCUS.x - INTRO_SHIP_HALF.x +
          (INTRO_CLOSEUP_SWEEP * fromMiddle * fromMiddle * fromMiddle),
      INTRO_CLOSEUP_FOCUS.y + INTRO_CLOSEUP_OFFSETS.at(pilot) -
          INTRO_SHIP_HALF.y +
          std::round(std::sin(through * INTRO_CLOSEUP_WOBBLE)));
}

std::optional<sf::Vector2f> squadronPlaceAt(std::size_t pilot, float scene) {
  if (scene < INTRO_SWARM_CUT) {
    return std::nullopt;
  }
  const sf::Vector2f swap = swapShiftAt(pilot, scene);
  return sf::Vector2f(
      entryXAt(pilot, scene) + swap.x + chargeShiftAt(scene),
      INTRO_HOMES.at(pilot).y + swap.y + swayAt(scene) + bobAt(pilot, scene));
}

std::optional<sf::Vector2f> shipPlaceAt(std::size_t pilot, float scene) {
  if (const std::optional<sf::Vector2f> closeup =
          closeupPlaceAt(pilot, scene)) {
    return closeup;
  }
  return squadronPlaceAt(pilot, scene);
}

std::optional<sf::Vector2f> landedPlaceAt(std::size_t pilot, float scene,
                                          sf::Vector2i squadronBox) {
  const std::optional<sf::Vector2f> place = shipPlaceAt(pilot, scene);
  if (!place.has_value()) {
    return std::nullopt;
  }
  const float landed = landingAt(pilot, scene);
  const sf::Vector2f target = sf::Vector2f(squadronBox) + titlePlaceOf(pilot);
  return sf::Vector2f(std::lerp(place->x, target.x, landed),
                      std::lerp(place->y, target.y, landed));
}

float exhaustAt(std::size_t pilot, float scene) {
  if (scene < INTRO_SWARM_CUT) {
    return INTRO_CLOSEUP_EXHAUST;
  }
  if (scene >= INTRO_SCENE_BOOST) {
    return INTRO_BOOST_EXHAUST * (1.0F - landingAt(pilot, scene));
  }
  if (scene >= INTRO_SCENE_CHARGE) {
    return INTRO_CHARGE_EXHAUST;
  }
  return std::lerp(
      INTRO_ENTRY_EXHAUST, INTRO_SWARM_EXHAUST,
      progressThrough(scene, INTRO_SWARM_CUT, INTRO_EXHAUST_SETTLE));
}

float smearAt(std::size_t pilot, float scene) {
  if (scene < INTRO_SWARM_CUT) {
    return 0.0F;
  }
  const float landing =
      scene >= landingStartOf(pilot) ? 1.0F - landingAt(pilot, scene) : 0.0F;
  return std::max(1.0F - progressThrough(scene, entryStartOf(pilot),
                                         INTRO_ENTRY_SMEAR_FADE),
                  landing);
}

sf::Vector2f noseOf(sf::Vector2f place) { return place + INTRO_NOSE; }

}  // namespace rtype::client
