#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include "ArmMount.hpp"
#include "ArmSide.hpp"
#include "DockConstants.hpp"
#include "InterfaceConstants.hpp"

namespace rtype::client {

/** @brief The chain of an arm, after a six-axis industrial arm (KUKA KR
 * QUANTEC): the column of A1 rises SHOULDER_RISE from the base, the link arm
 * A2 is UPPER_ARM_LENGTH long, the arm A3 FOREARM_LENGTH, and the in-line
 * wrist A4 to A6 holds the gripper GRIPPER_REACH before what it holds. */
constexpr int SHOULDER_RISE = 14;
constexpr float UPPER_ARM_LENGTH = 66.0F;
constexpr float FOREARM_LENGTH = 60.0F;
constexpr float GRIPPER_REACH = 26.0F;
constexpr int UPPER_ARM_WIDTH = 13;
constexpr int FOREARM_WIDTH = 10;
constexpr int SHOULDER_RADIUS = 11;
constexpr int ELBOW_RADIUS = 9;
constexpr int WRIST_RADIUS = 6;
constexpr float COLLAR_START = 0.74F;
constexpr float COLLAR_END = 0.82F;
constexpr float PISTON_REACH = 0.5F;
constexpr int PISTON_ROD_WIDTH = 2;
constexpr int PISTON_BODY_WIDTH = 4;
constexpr int PISTON_SIDE_OFFSET = 13;
constexpr float PISTON_BODY_SHARE = 0.55F;

/** @brief The gripper, after the Latching End Effector of Canadarm2: a drum
 * DRUM_RADIUS wide DRUM_OFFSET past the wrist, and three jaws. */
constexpr float DRUM_OFFSET = 7.0F;
constexpr int DRUM_RADIUS = 8;
constexpr float JAW_ROOT = 6.0F;
constexpr float JAW_SIDE = 5.0F;
constexpr float JAW_LENGTH = 6.0F;
constexpr float CLAW_LENGTH = 5.0F;
constexpr float CLOSED_JAW_SPREAD = 2.0F;
constexpr float OPENING_JAW_SPREAD = 6.0F;
constexpr float CLAW_HOOK = 3.0F;
/** @brief How much less a claw hooks in once its jaws are wide open. */
constexpr float CLAW_HOOK_OPENING = 2.0F;
constexpr std::array<int, 3> JAW_SIDES{-1, 0, 1};
constexpr int JAW_OUTLINE_WIDTH = 5;
constexpr int CLAW_OUTLINE_WIDTH = 4;
constexpr int JAW_WIDTH = 3;
constexpr int CLAW_WIDTH = 2;
constexpr int DRUM_CORE_RADIUS = 2;

/** @brief How far inside the edge of a joint its ink ring and its colored
 * core start. */
constexpr int JOINT_RING_INSET = 4;
constexpr int JOINT_CORE_INSET = 5;

/** @brief The base of an arm: a plate BASE_PLATE_SIZE large, banded with
 * hazard stripes HAZARD_STRIPE wide, a column COLUMN_WIDTH wide, a light. */
constexpr sf::Vector2i BASE_PLATE_SIZE{36, 7};
constexpr int HAZARD_STRIPE = 2;
constexpr int COLUMN_WIDTH = 22;
constexpr sf::Vector2i BASE_LIGHT_SIZE{6, 2};

/** @brief The four arms, a base in each corner of the bay, ARM_BASE_INSET in
 * from its sides; the bend keeps each elbow inside the bay. */
constexpr int ARM_BASE_INSET = 34;
constexpr ArmMount CEILING_LEFT_ARM{
    .base = {DOCK_AREA.position.x + ARM_BASE_INSET, DOCK_AREA.position.y},
    .side = ArmSide::Ceiling,
    .bend = 1,
    .grip = {5, 5}};
constexpr ArmMount CEILING_RIGHT_ARM{
    .base = {DOCK_AREA.position.x + DOCK_AREA.size.x - ARM_BASE_INSET,
             DOCK_AREA.position.y},
    .side = ArmSide::Ceiling,
    .bend = -1,
    .grip = {25, 5}};
constexpr ArmMount FLOOR_LEFT_ARM{
    .base = {DOCK_AREA.position.x + ARM_BASE_INSET,
             DOCK_AREA.position.y + DOCK_AREA.size.y},
    .side = ArmSide::Floor,
    .bend = -1,
    .grip = {5, 11}};
constexpr ArmMount FLOOR_RIGHT_ARM{
    .base = {DOCK_AREA.position.x + DOCK_AREA.size.x - ARM_BASE_INSET,
             DOCK_AREA.position.y + DOCK_AREA.size.y},
    .side = ArmSide::Floor,
    .bend = 1,
    .grip = {24, 10}};
constexpr std::array DOCK_ARMS{CEILING_LEFT_ARM, CEILING_RIGHT_ARM,
                               FLOOR_LEFT_ARM, FLOOR_RIGHT_ARM};
constexpr std::size_t CEILING_LEFT_ARM_INDEX = 0;
constexpr std::size_t CEILING_RIGHT_ARM_INDEX = 1;
constexpr std::size_t FLOOR_LEFT_ARM_INDEX = 2;
constexpr std::size_t FLOOR_RIGHT_ARM_INDEX = 3;

/** @brief Where a fetching arm reaches off screen: ARM_AWAY_BEYOND past the
 * right edge of the picture, ARM_AWAY_DROP into the bay from its side. */
constexpr int ARM_AWAY_BEYOND = 60;
constexpr int ARM_AWAY_DROP = 70;
constexpr int ARM_AWAY_X = INTERFACE_AREA.size.x + ARM_AWAY_BEYOND;

}  // namespace rtype::client
