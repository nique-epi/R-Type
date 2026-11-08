#include "ArmPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
#include "ArmMount.hpp"
#include "ArmPose.hpp"
#include "ArmSide.hpp"
#include "BackgroundColorConstants.hpp"
#include "ColoredPixel.hpp"
#include "DockArmConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "PixelDisc.hpp"
#include "PixelRounding.hpp"
#include "PixelSegment.hpp"
#include "PixelShapes.hpp"
#include "RenderingConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The joints of an arm in a frame. */
struct ArmJoints {
  sf::Vector2i shoulder;
  sf::Vector2i elbow;
  sf::Vector2i wrist;
};

/** @returns -1 for an arm that hangs from the ceiling, 1 for one on the
 * floor: the way its column rises. */
int riseOf(ArmSide side) { return side == ArmSide::Ceiling ? -1 : 1; }

/** @returns The unit vector from @p start to @p end; to the right when
 * they are the same pixel. */
sf::Vector2f directionOf(sf::Vector2i start, sf::Vector2i end) {
  const sf::Vector2f way(end - start);
  const float length = way.length();
  return length > 0.0F ? way / length : sf::Vector2f(1.0F, 0.0F);
}

sf::Vector2i elbowOf(sf::Vector2i shoulder, sf::Vector2i wrist, int bend) {
  const sf::Vector2f reach(wrist - shoulder);
  const float distance = std::clamp(reach.length(), 1.0F,
                                    UPPER_ARM_LENGTH + FOREARM_LENGTH - 1.0F);
  const float cosine =
      ((UPPER_ARM_LENGTH * UPPER_ARM_LENGTH) + (distance * distance) -
       (FOREARM_LENGTH * FOREARM_LENGTH)) /
      (2.0F * UPPER_ARM_LENGTH * distance);
  const float angle =
      std::atan2(reach.y, reach.x) +
      (static_cast<float>(bend) * std::acos(std::clamp(cosine, -1.0F, 1.0F)));
  return shoulder + nearestPixel({std::cos(angle) * UPPER_ARM_LENGTH,
                                  std::sin(angle) * UPPER_ARM_LENGTH});
}

/** @returns Where the column of @p mount ends: its shoulder, A2. */
sf::Vector2i shoulderOf(ArmMount mount) {
  return mount.base + sf::Vector2i(0, riseOf(mount.side) * -SHOULDER_RISE);
}

ArmJoints jointsOf(ArmMount mount, sf::Vector2i target) {
  const sf::Vector2i shoulder = shoulderOf(mount);
  const sf::Vector2i wrist =
      target - nearestPixel(directionOf(shoulder, target) * GRIPPER_REACH);
  return {.shoulder = shoulder,
          .elbow = elbowOf(shoulder, wrist, mount.bend),
          .wrist = wrist};
}

void addLink(std::vector<ColoredPixel>& pixels, PixelSegment link) {
  const int width = link.thickness;
  addSegment(
      pixels,
      {.from = link.from, .to = link.to, .thickness = width + 2, .color = INK});
  addSegment(pixels, {.from = link.from,
                      .to = link.to,
                      .thickness = width,
                      .color = PALE_BLUE});
  const sf::Vector2i lift(0, -1);
  addSegment(pixels, {.from = link.from + lift,
                      .to = link.to + lift,
                      .thickness = width - 3,
                      .color = link.color});
  addSegment(pixels, {.from = pixelAlong(link.from, link.to, COLLAR_START),
                      .to = pixelAlong(link.from, link.to, COLLAR_END),
                      .thickness = width,
                      .color = STAR_GOLD});
}

/** @brief Adds a white disc of the radius of @p disc, outlined in ink,
 * shaded pale blue on its bottom right: the body of a joint or a drum. */
void addShadedDisc(std::vector<ColoredPixel>& pixels, PixelDisc disc) {
  const sf::Vector2i lit(-1, -1);
  addDisc(pixels,
          {.center = disc.center, .radius = disc.radius + 1, .color = INK});
  addDisc(pixels,
          {.center = disc.center, .radius = disc.radius, .color = PALE_BLUE});
  addDisc(pixels, {.center = disc.center + lit,
                   .radius = disc.radius - 1,
                   .color = STAR_WHITE});
}

void addJoint(std::vector<ColoredPixel>& pixels, PixelDisc joint) {
  addShadedDisc(pixels, joint);
  addDisc(pixels, {.center = joint.center,
                   .radius = std::max(1, joint.radius - JOINT_RING_INSET),
                   .color = INK});
  addDisc(pixels, {.center = joint.center,
                   .radius = std::max(0, joint.radius - JOINT_CORE_INSET),
                   .color = joint.color});
}

void addPiston(std::vector<ColoredPixel>& pixels, ArmMount mount,
               const ArmJoints& joints) {
  const sf::Vector2i foot =
      joints.shoulder +
      sf::Vector2i(PISTON_SIDE_OFFSET * mount.bend,
                   riseOf(mount.side) * -(SHOULDER_RISE / HALF_DIVISOR));
  const sf::Vector2i head =
      pixelAlong(joints.shoulder, joints.elbow, PISTON_REACH);
  addSegment(pixels, {.from = foot,
                      .to = head,
                      .thickness = PISTON_BODY_WIDTH + 2,
                      .color = INK});
  addSegment(pixels, {.from = foot,
                      .to = head,
                      .thickness = PISTON_ROD_WIDTH,
                      .color = HUD_CYAN});
  addSegment(pixels, {.from = foot,
                      .to = pixelAlong(foot, head, PISTON_BODY_SHARE),
                      .thickness = PISTON_BODY_WIDTH,
                      .color = PALE_BLUE});
}

/** @brief The three strokes of a jaw, and whether it grips the hull. */
struct JawStroke {
  sf::Vector2f root;
  sf::Vector2f knuckle;
  sf::Vector2f claw;
  bool gripping{};
};

/** @brief Adds the drum of the gripper at @p drum: white, its core cyan
 * while it grips the hull. */
void addDrum(std::vector<ColoredPixel>& pixels, sf::Vector2i drum,
             bool gripping) {
  addShadedDisc(pixels,
                {.center = drum, .radius = DRUM_RADIUS, .color = STAR_WHITE});
  addDisc(pixels, {.center = drum,
                   .radius = DRUM_CORE_RADIUS,
                   .color = gripping ? HUD_CYAN : DEEP_BLUE});
}

/** @brief Adds a jaw: its root to its knuckle in white, its knuckle to its
 * claw in gold while it grips, both outlined in ink. */
void addJaw(std::vector<ColoredPixel>& pixels, const JawStroke& jaw) {
  const sf::Vector2i root = nearestPixel(jaw.root);
  const sf::Vector2i knuckle = nearestPixel(jaw.knuckle);
  const sf::Vector2i claw = nearestPixel(jaw.claw);
  addSegment(pixels, {.from = root,
                      .to = knuckle,
                      .thickness = JAW_OUTLINE_WIDTH,
                      .color = INK});
  addSegment(pixels, {.from = knuckle,
                      .to = claw,
                      .thickness = CLAW_OUTLINE_WIDTH,
                      .color = INK});
  addSegment(pixels, {.from = root,
                      .to = knuckle,
                      .thickness = JAW_WIDTH,
                      .color = STAR_WHITE});
  addSegment(pixels, {.from = knuckle,
                      .to = claw,
                      .thickness = CLAW_WIDTH,
                      .color = jaw.gripping ? STAR_GOLD : PALE_BLUE});
}

void addGripper(std::vector<ColoredPixel>& pixels, sf::Vector2i wrist,
                ArmPose pose) {
  const sf::Vector2f along = directionOf(wrist, pose.target);
  const sf::Vector2f across(-along.y, along.x);
  const sf::Vector2i drum = wrist + nearestPixel(along * DRUM_OFFSET);
  addDrum(pixels, drum, pose.gripping);
  const float spread = CLOSED_JAW_SPREAD + (pose.open * OPENING_JAW_SPREAD);
  const float hook = CLAW_HOOK - (pose.open * CLAW_HOOK_OPENING);
  for (const int side : JAW_SIDES) {
    const auto sideward = static_cast<float>(side);
    const sf::Vector2f root = sf::Vector2f(drum) + (along * JAW_ROOT) +
                              (across * (sideward * JAW_SIDE));
    const sf::Vector2f knuckle =
        root + (along * JAW_LENGTH) + (across * (sideward * spread));
    addJaw(pixels, {.root = root,
                    .knuckle = knuckle,
                    .claw = knuckle + (along * CLAW_LENGTH) -
                            (across * (sideward * hook)),
                    .gripping = pose.gripping});
  }
}

void paintBase(IInterfaceCanvas& canvas, ArmMount mount, bool gripping) {
  const sf::Vector2i shoulder = shoulderOf(mount);
  const int plateTop = mount.side == ArmSide::Floor
                           ? mount.base.y - BASE_PLATE_SIZE.y
                           : mount.base.y;
  const sf::IntRect plate(
      {mount.base.x - (BASE_PLATE_SIZE.x / HALF_DIVISOR), plateTop},
      BASE_PLATE_SIZE);
  canvas.fill(plate, INK);
  for (int stripe = 1; stripe < plate.size.x - 1;
       stripe += HAZARD_STRIPE + HAZARD_STRIPE) {
    canvas.fill({{plate.position.x + stripe, plate.position.y + 1},
                 {HAZARD_STRIPE, plate.size.y - 2}},
                STAR_GOLD);
  }
  const int columnTop = std::min(mount.base.y, shoulder.y);
  const sf::IntRect column(
      {mount.base.x - (COLUMN_WIDTH / HALF_DIVISOR), columnTop},
      {COLUMN_WIDTH, std::abs(mount.base.y - shoulder.y)});
  canvas.fill(column, INK);
  canvas.fill(grownBy(column, -1), STAR_WHITE);
  canvas.fill({{mount.base.x - (BASE_LIGHT_SIZE.x / HALF_DIVISOR),
                column.position.y + (column.size.y / HALF_DIVISOR)},
               BASE_LIGHT_SIZE},
              gripping ? PLAYER_GREEN : WARNING_ORANGE);
}

}  // namespace

void paintArm(IInterfaceCanvas& canvas, ArmMount mount, ArmPose pose) {
  const ArmJoints joints = jointsOf(mount, pose.target);
  paintBase(canvas, mount, pose.gripping);
  std::vector<ColoredPixel> pixels;
  addPiston(pixels, mount, joints);
  addLink(pixels, {.from = joints.shoulder,
                   .to = joints.elbow,
                   .thickness = UPPER_ARM_WIDTH,
                   .color = STAR_WHITE});
  addLink(pixels, {.from = joints.elbow,
                   .to = joints.wrist,
                   .thickness = FOREARM_WIDTH,
                   .color = STAR_WHITE});
  const sf::Color core = pose.gripping ? HUD_CYAN : DEEP_BLUE;
  addJoint(
      pixels,
      {.center = joints.shoulder, .radius = SHOULDER_RADIUS, .color = core});
  addJoint(pixels,
           {.center = joints.elbow, .radius = ELBOW_RADIUS, .color = core});
  addJoint(pixels,
           {.center = joints.wrist, .radius = WRIST_RADIUS, .color = core});
  addGripper(pixels, joints.wrist, pose);
  canvas.plot(pixels);
}

}  // namespace rtype::client
