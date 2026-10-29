#include "GroundEffects.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "ColorRamp.hpp"
#include "ComponentRegistry.hpp"
#include "Easing.hpp"
#include "Entity.hpp"
#include "GroundEffectConstants.hpp"
#include "IPixelSurface.hpp"
#include "Particle.hpp"
#include "PixelCircle.hpp"
#include "PixelRounding.hpp"
#include "Position.hpp"
#include "ShipFlight.hpp"
#include "Shockwave.hpp"
#include "SpeedLine.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The share of its life an effect has lived, 0 to 1. */
float shareOfLife(engine::Duration age, engine::Duration life) {
  if (life <= engine::Duration::zero()) {
    return 1.0F;
  }
  return std::clamp(shareOf(age, life), 0.0F, 1.0F);
}

/** @returns The pixel of the canvas under a position of the playfield. */
sf::Vector2i pixelOf(game::Position position) {
  return nearestPixel(sf::Vector2f(position.x, position.y) /
                      BACKGROUND_PIXEL_SIZE);
}

void paintParticles(engine::ComponentRegistry& components,
                    IPixelSurface& surface) {
  components.forEach<Particle, game::Position>(
      [&surface](engine::Entity, const Particle& particle,
                 const game::Position& position) {
        if (particle.age < engine::Duration::zero() ||
            particle.colors.empty()) {
          return;
        }
        const float share = shareOfLife(particle.age, particle.life);
        const sf::Color color = colorAt(particle.colors, share);
        const sf::Vector2i pixel = pixelOf(position);
        const int size =
            particle.shrinks && share < PARTICLE_SHRINK_SHARE ? 2 : 1;
        for (int row = 0; row < size; ++row) {
          for (int column = 0; column < size; ++column) {
            surface.plot(pixel + sf::Vector2i(column, row), color);
          }
        }
      });
}

void paintShockwaves(engine::ComponentRegistry& components,
                     IPixelSurface& surface) {
  components.forEach<Shockwave, game::Position>(
      [&surface](engine::Entity, const Shockwave& shockwave,
                 const game::Position& position) {
        if (shockwave.age < engine::Duration::zero() ||
            shockwave.colors.empty()) {
          return;
        }
        const float share = shareOfLife(shockwave.age, shockwave.life);
        const int radius = std::max(
            1,
            static_cast<int>(std::lround(static_cast<float>(shockwave.radius) *
                                         easeOutCubic(share))));
        const sf::Color color = colorAt(shockwave.colors, share);
        const sf::Vector2i center = pixelOf(position);
        const bool dotted = share > 1.0F - SHOCKWAVE_DOTTED_SHARE;
        for (const sf::Vector2i offset : circleOf(radius)) {
          const sf::Vector2i pixel = center + offset;
          if (!dotted || (pixel.x + pixel.y) % 2 == 0) {
            surface.plot(pixel, color);
          }
        }
      });
}

void paintSpeedLines(engine::ComponentRegistry& components,
                     IPixelSurface& surface) {
  components.forEach<SpeedLine, game::Position>(
      [&surface](engine::Entity, const SpeedLine& line,
                 const game::Position& position) {
        if (line.age < engine::Duration::zero()) {
          return;
        }
        const float share = shareOfLife(line.age, line.life);
        const sf::Vector2i end = pixelOf(position);
        const auto length = static_cast<float>(end.x);
        const int head = static_cast<int>(
            std::lround(length * easeOutCubic(share * SPEED_LINE_HEAD_PACE)));
        const int tail = static_cast<int>(
            std::lround(length * easeOutCubic((share * SPEED_LINE_HEAD_PACE) -
                                              SPEED_LINE_TAIL_DELAY)));
        for (int column = tail; column < head; ++column) {
          surface.plot({column, end.y}, line.color);
        }
      });
}

/** @brief One flight line of a ship: its row of the canvas, the column after
 * its last pixel, and its length. */
struct FlightLine {
  int row{};
  int right{};
  int length{};
};

/** @returns How many pixels the flame of @p flight burns behind the engine. */
int flameLengthOf(const ShipFlight& flight) {
  const int flicker = (flight.age / FLAME_FLICKER_STEP) % 2 == 0 ? 0 : 1;
  return 1 + pixelsOf(FLAME_LONGEST - 1, flight.flame) + flicker;
}

/** @brief Paints the flame of every ship that has a ShipFlight behind its
 * engine, on the two rows of the engine, its tip on the upper one. */
void paintFlames(engine::ComponentRegistry& components,
                 IPixelSurface& surface) {
  components.forEach<ShipFlight, game::Position>(
      [&surface](engine::Entity, const ShipFlight& flight,
                 const game::Position& position) {
        const sf::Vector2i nozzle =
            pixelOf(position) - sf::Vector2i(SHIP_HALF_WIDTH_PIXELS + 1, 1);
        const int length = flameLengthOf(flight);
        for (int step = 0; step < length; ++step) {
          const float share = static_cast<float>(step) /
                              static_cast<float>(std::max(length - 1, 1));
          const sf::Color color = colorAt(FLAME_COLORS, share);
          surface.plot(nozzle - sf::Vector2i(step, 0), color);
          if (share < FLAME_TIP_SHARE) {
            surface.plot(nozzle - sf::Vector2i(step, -1), color);
          }
        }
      });
}

/** @brief Paints @p line in PALE_BLUE. */
void paintFlightLine(IPixelSurface& surface, FlightLine line) {
  for (int column = line.right - line.length; column < line.right; ++column) {
    surface.plot({column, line.row}, PALE_BLUE);
  }
}

/** @brief Paints the flight lines of every ShipFlight whose pilot has pushed
 * forward for FLIGHT_LINES_AFTER or dashes. */
void paintFlightLines(engine::ComponentRegistry& components,
                      IPixelSurface& surface) {
  components.forEach<ShipFlight, game::Position>(
      [&surface](engine::Entity, const ShipFlight& flight,
                 const game::Position& position) {
        if (flight.forwardTime < FLIGHT_LINES_AFTER && !flight.dashing) {
          return;
        }
        const sf::Vector2i center = pixelOf(position);
        const auto scrolled =
            static_cast<int>(std::chrono::duration<float>(flight.age).count() *
                             FLIGHT_LINE_PACE);
        for (std::size_t index = 0; index < SPEED_LINE_ROWS.size(); ++index) {
          const int phase =
              (scrolled + FLIGHT_LINE_PHASES.at(index)) % FLIGHT_LINE_PERIOD;
          paintFlightLine(
              surface, FlightLine{.row = center.y + SPEED_LINE_ROWS.at(index),
                                  .right = center.x - SHIP_HALF_WIDTH_PIXELS -
                                           FLIGHT_LINE_GAP - phase,
                                  .length = FLIGHT_LINE_LENGTHS.at(index)});
        }
      });
}

}  // namespace

void paintGroundEffects(engine::ComponentRegistry& components,
                        IPixelSurface& surface) {
  paintShockwaves(components, surface);
  paintSpeedLines(components, surface);
  paintFlightLines(components, surface);
  paintFlames(components, surface);
  paintParticles(components, surface);
}

}  // namespace rtype::client
