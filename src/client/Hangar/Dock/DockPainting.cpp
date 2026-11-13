#include "DockPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <optional>
#include <string>
#include <vector>
#include "ArmPainting.hpp"
#include "ArmPose.hpp"
#include "BackgroundColorConstants.hpp"
#include "ColoredPixel.hpp"
#include "DitherPatch.hpp"
#include "DockArmConstants.hpp"
#include "DockConstants.hpp"
#include "DockScene.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "FittingRoute.hpp"
#include "FittingRun.hpp"
#include "FittingTimeline.hpp"
#include "FittingTimelineConstants.hpp"
#include "HangarBayConstants.hpp"
#include "HangarColorConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "MetalPatch.hpp"
#include "MetalTexture.hpp"
#include "MetalTextureConstants.hpp"
#include "MotionPreference.hpp"
#include "Outline.hpp"
#include "PilotShipPicture.hpp"
#include "PixelShapes.hpp"
#include "RenderingConstants.hpp"
#include "ShipHulls.hpp"
#include "ShipLook.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookNumbers.hpp"
#include "ShipLookSlot.hpp"
#include "ShipLookSpriteConstants.hpp"
#include "ShipLooks.hpp"
#include "ShipPicture.hpp"
#include "ShipPlacement.hpp"
#include "ShipSheetFrame.hpp"
#include "SpriteFrame.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

using identifiers::SHIP_LOOK_ITEM_SLOTS;
using identifiers::ShipLookSlot;

namespace {

using StillPoses = std::array<std::optional<ArmPose>, DOCK_ARMS.size()>;

/** @returns Where the ship stands: centred in the bay, its top on
 * DOCK_SHIP_TOP, a pixel higher every other DOCK_BOB_STEP. */
ShipPlacement dockedShipOf(const DockScene& scene) {
  const int bob =
      scene.motion == MotionPreference::Full
          ? static_cast<int>((scene.elapsed / DOCK_BOB_STEP) % DOCK_BOB_STEPS)
          : 0;
  return {.topLeft = {DOCK_AREA.position.x +
                          ((DOCK_AREA.size.x -
                            (PLAYER_SHIP_FRAME.width * DOCK_SHIP_SCALE)) /
                           HALF_DIVISOR),
                      DOCK_SHIP_TOP - bob},
          .scale = DOCK_SHIP_SCALE,
          .visibleArea = DOCK_AREA};
}

/** @returns Whether @p item is brought from below: the mustache, held by
 * its bottom. */
bool comesFromBelow(std::size_t item) {
  return SHIP_LOOK_ITEM_SLOTS.at(item) == ShipLookSlot::Face;
}

/** @returns The arm that fetches @p item: the top right one for a hat or
 * shades, held from above; the bottom right one for the mustache. */
std::size_t fittingArmOf(std::size_t item) {
  return comesFromBelow(item) ? FLOOR_RIGHT_ARM_INDEX : CEILING_RIGHT_ARM_INDEX;
}

/** @returns Where the jaws hold @p item, from the top-left corner of its
 * drawing on a ship at @p scale: the middle of its top, or of its bottom
 * when the arm comes from below. */
sf::Vector2i holdOf(std::size_t item, int scale) {
  const sf::Vector2i size = SHIP_LOOK_SPRITES.at(item).size * scale;
  return {size.x / HALF_DIVISOR, comesFromBelow(item) ? size.y - 1 : 1};
}

/** @returns Where the top-left corner of @p item lies on the ship. */
sf::Vector2i mountOf(std::size_t item, ShipPlacement ship) {
  return ship.topLeft + (SHIP_LOOK_SPRITES.at(item).mount * ship.scale);
}

/** @returns Where @p arm grips the ship. */
sf::Vector2i gripOf(std::size_t arm, ShipPlacement ship) {
  return ship.topLeft + (DOCK_ARMS.at(arm).grip * ship.scale);
}

FittingRoute routeOf(std::size_t item, ShipPlacement ship) {
  const bool fromBelow = comesFromBelow(item);
  const int awayY =
      fromBelow ? DOCK_AREA.position.y + DOCK_AREA.size.y - ARM_AWAY_DROP
                : DOCK_AREA.position.y + ARM_AWAY_DROP;
  return {.grip = gripOf(fittingArmOf(item), ship),
          .away = {ARM_AWAY_X, awayY},
          .place = mountOf(item, ship) + holdOf(item, ship.scale),
          .approach = fromBelow ? FITTING_APPROACH : -FITTING_APPROACH};
}

/** @returns The pose of each arm the fitting does not move: gripping the
 * hull; none for the arm that fits an item. */
StillPoses stillPosesOf(const DockScene& scene, ShipPlacement ship) {
  StillPoses still{};
  for (std::size_t arm = 0; arm < DOCK_ARMS.size(); ++arm) {
    if (!scene.fitting.has_value() ||
        fittingArmOf(scene.fitting->item) != arm) {
      still.at(arm) = ArmPose{.target = gripOf(arm, ship), .gripping = true};
    }
  }
  return still;
}

void paintSparks(IInterfaceCanvas& canvas, sf::Vector2i center, float share) {
  if (share <= 0.0F || share >= 1.0F) {
    return;
  }
  const std::array colors{STAR_WHITE, STAR_GOLD, HUD_CYAN};
  const float radius = SPARK_FIRST_RADIUS + (share * SPARK_SPREAD);
  for (int spark = 0; spark < SPARK_COUNT; ++spark) {
    const float angle =
        (static_cast<float>(spark) * 2.0F * std::numbers::pi_v<float> /
         static_cast<float>(SPARK_COUNT)) +
        share;
    canvas.fill(
        {center + sf::Vector2i(static_cast<int>(std::cos(angle) * radius),
                               static_cast<int>(std::sin(angle) * radius *
                                                SPARK_FLATTENING)),
         {SPARK_SIZE, SPARK_SIZE}},
        colors.at(static_cast<std::size_t>(spark) % colors.size()));
  }
}

/** @returns The middle of @p item on the ship. */
sf::Vector2i middleOf(std::size_t item, ShipPlacement ship) {
  return mountOf(item, ship) +
         ((SHIP_LOOK_SPRITES.at(item).size * ship.scale) / HALF_DIVISOR);
}

/** @brief Paints the sparks of the old item of the slot as it goes, then of
 * the new one as it is set down. */
void paintFittingSparks(IInterfaceCanvas& canvas, const DockScene& scene,
                        ShipPlacement ship) {
  if (!scene.fitting.has_value()) {
    return;
  }
  const FittingRun& run = *scene.fitting;
  const float progress = fittingProgressOf(run, scene.elapsed);
  const std::optional<std::size_t> old =
      itemInSlot(run.before, SHIP_LOOK_ITEM_SLOTS.at(run.item));
  if (old.has_value() && *old != run.item) {
    paintSparks(canvas, middleOf(*old, ship),
                sparksShareAt(progress, FITTING_HOVER_END));
  }
  paintSparks(canvas, middleOf(run.item, ship),
              sparksShareAt(progress, FITTING_PLACE_AT));
}

/** @brief Paints the arm that fits an item, with the item under its jaws
 * while it carries it. */
void paintFittingArm(IInterfaceCanvas& canvas, const DockScene& scene,
                     ShipPlacement ship) {
  if (!scene.fitting.has_value()) {
    return;
  }
  const FittingRun& run = *scene.fitting;
  const std::size_t item = run.item;
  const ArmPose pose =
      fittingPoseAt(fittingProgressOf(run, scene.elapsed), routeOf(item, ship));
  if (pose.carrying) {
    const sf::Vector2i topLeft = pose.target - holdOf(item, ship.scale);
    paintLookItem(
        canvas, item,
        {.topLeft = topLeft - (SHIP_LOOK_SPRITES.at(item).mount * ship.scale),
         .scale = ship.scale,
         .visibleArea = DOCK_AREA});
  }
  paintArm(canvas, DOCK_ARMS.at(fittingArmOf(item)), pose);
}

/** @brief Paints the arms of @p arms that hold a still pose. */
void paintStillArms(IInterfaceCanvas& canvas, const StillPoses& still,
                    std::array<std::size_t, 2> arms) {
  for (const std::size_t arm : arms) {
    if (const std::optional<ArmPose>& pose = still.at(arm); pose.has_value()) {
      paintArm(canvas, DOCK_ARMS.at(arm), *pose);
    }
  }
}

/** @brief Paints the ship, its engines running, and what it shows wearing:
 * what the settings say, or the step of the fitting under way. */
void paintDockedShip(IInterfaceCanvas& canvas, const DockScene& scene,
                     ShipPlacement ship) {
  const game::SpriteFrame frame =
      scene.motion == MotionPreference::Full
          ? PLAYER_SHIP_ANIMATION.frameAt(scene.elapsed)
                .value_or(PLAYER_SHIP_FRAME)
          : PLAYER_SHIP_FRAME;
  const identifiers::ShipLook shown =
      scene.fitting.has_value()
          ? lookShownAt(fittingProgressOf(*scene.fitting, scene.elapsed),
                        *scene.fitting)
          : scene.worn;
  paintPilotShip(canvas,
                 PilotShipPicture{.frame = frame,
                                  .color = scene.shipColor,
                                  .hull = shown.hull,
                                  .palette = scene.palette},
                 ship);
  paintShipLook(canvas, shown, ship);
}

/** @brief What the reference card draws: a rectangle of a sheet, and how
 * many times larger. */
struct BlueprintDrawing {
  TextureArea image;
  int scale{};
};

/** @returns The drawing of @p item on the card: its ship pixel for pixel for
 * a hull, its drawing of the item sheet at BLUEPRINT_DRAWING_SCALE else. */
BlueprintDrawing blueprintDrawingOf(std::size_t item) {
  if (identifiers::isHullItem(item)) {
    const ShipSheetFrame ship = hullItemFrameOf(item);
    return {.image = TextureArea{.assetId = ship.sheet,
                                 .rectangle = rectangleOf(ship.frame)},
            .scale = BLUEPRINT_HULL_SCALE};
  }
  return {.image = TextureArea{.assetId = SHIP_LOOK_SHEET_ID,
                               .rectangle = lookRectangleOf(item)},
          .scale = BLUEPRINT_DRAWING_SCALE};
}

/** @brief Paints the panels of the wall, brushed and riveted. */
void paintPanels(IInterfaceCanvas& canvas) {
  std::vector<ColoredPixel> pixels;
  for (int left = 0; left < DOCK_AREA.size.x; left += DOCK_PANEL_WIDTH) {
    const sf::IntRect panel(
        DOCK_AREA.position +
            sf::Vector2i(left + DOCK_PANEL_SEAM, DOCK_PANEL_SEAM),
        {std::min(DOCK_PANEL_WIDTH, DOCK_AREA.size.x - left) - DOCK_PANEL_SEAM -
             DOCK_PANEL_SEAM,
         DOCK_AREA.size.y - DOCK_PANEL_SEAM - DOCK_PANEL_SEAM});
    canvas.fill(panel, STEEL_DARK);
    addBrushStrokes(
        pixels,
        MetalPatch{.area = panel,
                   .shades = {.light = DEEP_BLUE, .dark = HANGAR_SHADOW},
                   .seed = static_cast<std::uint32_t>(left)});
    addRivet(pixels, panel.position + RIVET_OFFSET);
    addRivet(pixels, {panel.position.x + panel.size.x - RIVET_INSET.x,
                      panel.position.y + RIVET_OFFSET.y});
  }
  canvas.plot(pixels);
}

/** @returns The middle column of the bay. */
int dockMiddle() {
  return DOCK_AREA.position.x + (DOCK_AREA.size.x / HALF_DIVISOR);
}

/** @brief Paints the light falling from the ceiling, dithered, and the
 * beacons of the floor, @p litBeacon lit. */
void paintLightAndBeacons(IInterfaceCanvas& canvas,
                          std::optional<int> litBeacon) {
  std::vector<ColoredPixel> light;
  addDither(
      light,
      DitherPatch{
          .area = {{dockMiddle() - (DOCK_LIGHT_WIDTH / HALF_DIVISOR),
                    DOCK_AREA.position.y + DOCK_PANEL_SEAM},
                   {DOCK_LIGHT_WIDTH,
                    static_cast<int>(static_cast<float>(DOCK_AREA.size.y) *
                                     DOCK_LIGHT_SHARE)}},
          .color = DEEP_BLUE,
          .topShare = DOCK_LIGHT_SHARE,
          .bottomShare = 0.0F});
  canvas.plot(light);
  const int firstBeacon =
      dockMiddle() - ((BEACON_COUNT * BEACON_SPACING) / HALF_DIVISOR);
  for (int beacon = 0; beacon < BEACON_COUNT; ++beacon) {
    canvas.fill({{firstBeacon + (beacon * BEACON_SPACING), DOCK_BEACONS_TOP},
                 {BEACON_SIZE, BEACON_SIZE}},
                litBeacon == beacon ? BEACON_LIT_COLOR : BEACON_OFF_COLOR);
  }
}

/** @brief Paints the paper of the reference card: dark blue, a dotted grid,
 * a pale frame, its label. */
void paintBlueprintPaper(IInterfaceCanvas& canvas) {
  const sf::IntRect card = BLUEPRINT_AREA;
  canvas.fill(grownBy(card, 1), INK);
  canvas.fill(card, DEEP_BLUE);
  std::vector<ColoredPixel> grid;
  for (int row = BLUEPRINT_GRID_PITCH; row < card.size.y;
       row += BLUEPRINT_GRID_PITCH) {
    for (int column = 1; column < card.size.x;
         column += BLUEPRINT_GRID_DOT_STEP) {
      grid.push_back({.position = card.position + sf::Vector2i(column, row),
                      .color = BLUE});
    }
  }
  canvas.plot(grid);
  Outline(card, Outline::Line::Solid).paint(canvas, PALE_BLUE);
  canvas.write(BLUEPRINT_LABEL, card.position + BLUEPRINT_LABEL_OFFSET,
               {.font = TextFont::Small, .color = HUD_CYAN});
}

}  // namespace

std::optional<int> litBeaconOf(engine::Duration elapsed,
                               MotionPreference motion) {
  if (motion == MotionPreference::Reduced) {
    return std::nullopt;
  }
  return static_cast<int>((elapsed / BEACON_STEP) % BEACON_COUNT);
}

void paintDockWall(IInterfaceCanvas& canvas, std::optional<int> litBeacon) {
  canvas.fill(DOCK_AREA, HANGAR_SHADOW);
  paintPanels(canvas);
  paintLightAndBeacons(canvas, litBeacon);
}

void paintBlueprint(IInterfaceCanvas& canvas, std::size_t item) {
  paintBlueprintPaper(canvas);
  const sf::IntRect card = BLUEPRINT_AREA;
  const BlueprintDrawing drawing = blueprintDrawingOf(item);
  const sf::Vector2i size = drawing.image.rectangle.size;
  const sf::Vector2i topLeft(
      card.position.x +
          ((card.size.x - (size.x * drawing.scale)) / HALF_DIVISOR),
      card.position.y + BLUEPRINT_DRAWING_TOP);
  canvas.drawImage(
      drawing.image,
      ImagePlacement{
          .topLeft = topLeft, .scale = drawing.scale, .tint = HUD_CYAN},
      card);
  const int ruleTop = topLeft.y + (size.y * drawing.scale) + BLUEPRINT_RULE_GAP;
  canvas.fill({{topLeft.x, ruleTop}, {size.x * drawing.scale, 1}}, HUD_CYAN);
  canvas.write(
      std::to_string(size.x) + BLUEPRINT_SIZE_JOIN + std::to_string(size.y),
      {card.position.x + (card.size.x / HALF_DIVISOR),
       ruleTop + BLUEPRINT_RULE_GAP},
      {.font = TextFont::Small,
       .color = HUD_CYAN,
       .alignment = TextAlignment::Center});
}

void paintDock(IInterfaceCanvas& canvas, const DockScene& scene) {
  const ShipPlacement ship = dockedShipOf(scene);
  const StillPoses still = stillPosesOf(scene, ship);
  paintStillArms(canvas, still,
                 {CEILING_LEFT_ARM_INDEX, CEILING_RIGHT_ARM_INDEX});
  paintDockedShip(canvas, scene, ship);
  paintFittingSparks(canvas, scene, ship);
  paintStillArms(canvas, still, {FLOOR_LEFT_ARM_INDEX, FLOOR_RIGHT_ARM_INDEX});
  if (scene.fitting.has_value()) {
    paintFittingArm(canvas, scene, ship);
  }
}

}  // namespace rtype::client
