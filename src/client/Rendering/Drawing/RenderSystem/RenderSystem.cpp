#include "RenderSystem.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <vector>
#include "BackgroundConstants.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "DrawCall.hpp"
#include "Easing.hpp"
#include "Entity.hpp"
#include "HitFlash.hpp"
#include "IDrawSurface.hpp"
#include "Invulnerability.hpp"
#include "Layer.hpp"
#include "Nudge.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "Reveal.hpp"
#include "ShipFlight.hpp"
#include "ShipFlightConstants.hpp"
#include "Sprite.hpp"
#include "SpriteAppearance.hpp"
#include "SpriteDrawing.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Rows of a frame: the first one, from the top, and how many. */
struct RowSpan {
  int first{};
  int count{};
};

/** @returns Whether an invulnerable entity shows its dithered picture now:
 * always when steady, else on every odd INVULNERABILITY_BLINK_STEP. */
bool isDithered(const Invulnerability& invulnerability) {
  return invulnerability.steady ||
         (invulnerability.elapsed / INVULNERABILITY_BLINK_STEP) % 2 == 1;
}

/** @returns The tone of an entity: the tone of its hit flash, else the ghost
 * of an invulnerability in its dithered phase, else Normal. */
SpriteTone toneOf(const HitFlash* flash,
                  const Invulnerability* invulnerability) {
  if (flash != nullptr) {
    return flash->tone;
  }
  return invulnerability != nullptr && isDithered(*invulnerability)
             ? SpriteTone::Ghost
             : SpriteTone::Normal;
}

/** @returns The outline of a ship whose invulnerability is steady, or
 * nothing: no such invulnerability, or no outline chosen for it. */
std::optional<sf::Color> outlineOf(const Invulnerability* invulnerability) {
  if (invulnerability == nullptr || !invulnerability->steady) {
    return std::nullopt;
  }
  return invulnerability->outline;
}

/** @returns How many pixels of the canvas a nudge draws an entity away: its
 * amount, or minus its amount on every odd flip step; none without nudge. */
sf::Vector2i offsetOf(const Nudge* nudge) {
  if (nudge == nullptr) {
    return {};
  }
  const bool flipped = nudge->flipEvery > engine::Duration::zero() &&
                       (nudge->elapsed / nudge->flipEvery) % 2 == 1;
  return flipped ? -nudge->amount : nudge->amount;
}

/** @returns How many rows of a frame @p height rows tall a reveal shows: none
 * on the first half of its duration, then the share of the second half that
 * passed, rounded up; nothing without reveal. */
std::optional<int> shownRowsOf(const Reveal* reveal, int height) {
  if (reveal == nullptr) {
    return std::nullopt;
  }
  const double half = static_cast<double>(reveal->duration.count()) * HALF;
  const auto age = static_cast<double>(reveal->age.count());
  const double shown =
      half > 0.0 ? std::clamp((age - half) / half, 0.0, 1.0) : 1.0;
  return static_cast<int>(std::ceil(shown * static_cast<double>(height)));
}

/** @returns A position moved by an offset in pixels of the canvas. */
game::Position nudged(game::Position position, sf::Vector2i offset) {
  return game::Position{
      .x = position.x + (static_cast<float>(offset.x) * BACKGROUND_PIXEL_SIZE),
      .y = position.y + (static_cast<float>(offset.y) * BACKGROUND_PIXEL_SIZE)};
}

/** @returns The rows @p span of a frame, as a frame of their own. */
game::SpriteFrame rowsOf(game::SpriteFrame frame, RowSpan span) {
  return game::SpriteFrame{.left = frame.left,
                           .top = frame.top + span.first,
                           .width = frame.width,
                           .height = span.count};
}

/** @returns The center of the rows @p span of a frame @p height rows tall
 * centered on @p center: drawn there, they land where they lie in the whole
 * frame. */
game::Position centerOfRows(game::Position center, int height, RowSpan span) {
  const float rowsAboveTheirCenter =
      static_cast<float>(span.first) + (static_cast<float>(span.count) * HALF);
  return game::Position{.x = center.x,
                        .y = center.y + ((rowsAboveTheirCenter -
                                          (static_cast<float>(height) * HALF)) *
                                         BACKGROUND_PIXEL_SIZE)};
}

/** @brief The rows a roll shows of a frame: its height, how many it shows,
 * and whether they are upside down. */
struct RolledFrame {
  int height{};
  int count{};
  bool belly{false};
};

/** @returns The angle of the roll @p flight draws, a whole turn over its
 * share; nothing without a ShipFlight or a roll. */
std::optional<float> rollAngleOf(const ShipFlight* flight) {
  if (flight == nullptr || !flight->rollShare.has_value()) {
    return std::nullopt;
  }
  return FULL_TURN * *flight->rollShare;
}

/** @returns The rows of @p frame a roll at @p angle shows: round(height x
 * |cos|), ROLL_THINNEST_ROWS at least, upside down past a quarter turn. */
RolledFrame rolledFrameOf(game::SpriteFrame frame, float angle) {
  const float cosine = std::cos(angle);
  const int count = pixelsOf(frame.height, std::abs(cosine));
  return RolledFrame{.height = frame.height,
                     .count = std::max(ROLL_THINNEST_ROWS, count),
                     .belly = cosine < 0.0F};
}

/** @returns The row of the frame that row @p row of @p rolled shows: the
 * nearest, counted from the bottom while the belly shows. */
int sourceRowOf(RolledFrame rolled, int row) {
  const int nearest = (((2 * row) + 1) * rolled.height) / (2 * rolled.count);
  return rolled.belly ? rolled.height - 1 - nearest : nearest;
}

/** @returns The draw call of an afterimage of @p sprite, the sprite of
 * @p entity, at @p position: the sprite lit one pixel in two. */
DrawCall afterimageAt(engine::Entity entity, const game::Sprite& sprite,
                      game::Position position) {
  return DrawCall{.entityIndex = entity.index,
                  .position = position,
                  .sprite = &sprite,
                  .caption = nullptr,
                  .look = SpriteAppearance{.tone = SpriteTone::Ghost,
                                           .outline = std::nullopt,
                                           .offset = {},
                                           .shownRows = std::nullopt,
                                           .leadingLine = false,
                                           .rollAngle = std::nullopt}};
}

}  // namespace

void RenderSystem::render(engine::ComponentRegistry& components,
                          IDrawSurface& surface) {
  for (auto& drawCalls : drawCallsByLayer_) {
    drawCalls.clear();
  }
  collectSprites(components);
  collectAfterimages(components);
  collectCaptions(components);
  for (auto& drawCalls : drawCallsByLayer_) {
    std::ranges::sort(drawCalls, {}, &DrawCall::entityIndex);
    for (const DrawCall& drawCall : drawCalls) {
      draw(surface, drawCall);
    }
  }
}

void RenderSystem::collectSprites(engine::ComponentRegistry& components) {
  components.forEach<game::Position, game::Sprite>(
      [this, &components](engine::Entity entity, game::Position& position,
                          game::Sprite& sprite) {
        const auto layerIndex = static_cast<std::size_t>(sprite.layer);
        if (layerIndex >= drawCallsByLayer_.size()) {
          return;
        }
        drawCallsByLayer_[layerIndex].push_back(
            {.entityIndex = entity.index,
             .position = position,
             .sprite = &sprite,
             .caption = nullptr,
             .look = lookOf(components, entity, sprite.frame.height)});
      });
}

void RenderSystem::collectCaptions(engine::ComponentRegistry& components) {
  components.forEach<game::Position, Caption>([this](engine::Entity entity,
                                                     game::Position& position,
                                                     Caption& caption) {
    const auto layerIndex = static_cast<std::size_t>(caption.layer);
    if (!caption.visible || layerIndex >= drawCallsByLayer_.size()) {
      return;
    }
    drawCallsByLayer_[layerIndex].push_back({.entityIndex = entity.index,
                                             .position = position,
                                             .sprite = nullptr,
                                             .caption = &caption,
                                             .look = {}});
  });
}

SpriteAppearance RenderSystem::lookOf(engine::ComponentRegistry& components,
                                      engine::Entity entity, int frameHeight) {
  const Invulnerability* invulnerability =
      components.get<Invulnerability>(entity);
  const Reveal* reveal = components.get<Reveal>(entity);
  return SpriteAppearance{
      .tone = toneOf(components.get<HitFlash>(entity), invulnerability),
      .outline = outlineOf(invulnerability),
      .offset = offsetOf(components.get<Nudge>(entity)),
      .shownRows = shownRowsOf(reveal, frameHeight),
      .leadingLine = reveal != nullptr && reveal->leadingLine,
      .rollAngle = rollAngleOf(components.get<ShipFlight>(entity))};
}

void RenderSystem::draw(IDrawSurface& surface, const DrawCall& call) {
  if (call.caption != nullptr) {
    surface.write(call.caption->text, call.position, call.caption->style);
    return;
  }
  drawSprite(surface, call);
}

void RenderSystem::drawSprite(IDrawSurface& surface, const DrawCall& call) {
  const game::Position center = nudged(call.position, call.look.offset);
  if (call.look.rollAngle.has_value()) {
    drawRolled(surface, call, center);
    return;
  }
  if (call.look.shownRows.has_value()) {
    drawRevealed(surface, call, center);
    return;
  }
  surface.draw(SpriteDrawing{.assetId = call.sprite->assetId,
                             .frame = call.sprite->frame,
                             .center = center,
                             .tone = call.look.tone,
                             .outline = call.look.outline});
}

void RenderSystem::drawRevealed(IDrawSurface& surface, const DrawCall& call,
                                game::Position center) {
  const game::SpriteFrame frame = call.sprite->frame;
  const int rows = call.look.shownRows.value_or(frame.height);
  if (rows <= 0) {
    return;
  }
  const RowSpan shown{.first = 0, .count = rows};
  surface.draw(
      SpriteDrawing{.assetId = call.sprite->assetId,
                    .frame = rowsOf(frame, shown),
                    .center = centerOfRows(center, frame.height, shown),
                    .tone = call.look.tone,
                    .outline = std::nullopt});
  if (rows < frame.height && call.look.leadingLine) {
    const RowSpan line{.first = rows, .count = 1};
    surface.draw(
        SpriteDrawing{.assetId = call.sprite->assetId,
                      .frame = rowsOf(frame, line),
                      .center = centerOfRows(center, frame.height, line),
                      .tone = SpriteTone::Flash,
                      .outline = std::nullopt});
  }
}

void RenderSystem::collectAfterimages(engine::ComponentRegistry& components) {
  std::vector<DrawCall>& effects =
      drawCallsByLayer_[static_cast<std::size_t>(game::Layer::Effects)];
  components.forEach<ShipFlight, game::Sprite>([&effects](
                                                   engine::Entity entity,
                                                   ShipFlight& flight,
                                                   game::Sprite& sprite) {
    if (!flight.rolling && !flight.dashing) {
      return;
    }
    for (const std::size_t age : AFTERIMAGE_AGES) {
      if (age < flight.trailLength) {
        effects.push_back(afterimageAt(entity, sprite, flight.trail.at(age)));
      }
    }
  });
}

void RenderSystem::drawRolled(IDrawSurface& surface, const DrawCall& call,
                              game::Position center) {
  const game::SpriteFrame frame = call.sprite->frame;
  const RolledFrame rolled =
      rolledFrameOf(frame, call.look.rollAngle.value_or(0.0F));
  for (int row = 0; row < rolled.count; ++row) {
    surface.draw(SpriteDrawing{
        .assetId = call.sprite->assetId,
        .frame = rowsOf(frame,
                        RowSpan{.first = sourceRowOf(rolled, row), .count = 1}),
        .center = centerOfRows(center, rolled.count,
                               RowSpan{.first = row, .count = 1}),
        .tone = rolled.belly ? SpriteTone::Shade : call.look.tone,
        .outline = std::nullopt});
  }
}

}  // namespace rtype::client
