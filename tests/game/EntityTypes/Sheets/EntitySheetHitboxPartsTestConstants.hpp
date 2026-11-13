#pragma once

#include <cstddef>
#include <string_view>
#include "CollisionBox.hpp"

namespace rtype::game::testing {

/** @brief A hitbox that lists two boxes: a head of 40 by 40, 80 units left of
 * the Position of the body, from x -100 to -60 and from y -20 to 20, and a
 * trunk of 100 by 60, 30 units right of it and 50 below, from x -20 to 80 and
 * from y 20 to 80. */
constexpr std::string_view HITBOX_OF_TWO_BOXES =
    "[[gameplay.hitbox.box]]\nwidth = 40\nheight = 40\noffset_x = -80\n"
    "[[gameplay.hitbox.box]]\nwidth = 100\nheight = 60\noffset_x = 30\n"
    "offset_y = 50\n";
constexpr std::size_t TWO_BOXES = 2;
constexpr CollisionBox HEAD_OF_TWO_BOXES{
    .width = 40.0F, .height = 40.0F, .offsetX = -80.0F};
constexpr CollisionBox TRUNK_OF_TWO_BOXES{
    .width = 100.0F, .height = 60.0F, .offsetX = 30.0F, .offsetY = 50.0F};

/** @brief The rectangle that encloses those two boxes: from x -100 to 80 and
 * from y -20 to 80, so 180 by 100, its center 10 units left of the Position
 * and 30 below it. */
constexpr CollisionBox BOX_AROUND_TWO_BOXES{
    .width = 180.0F, .height = 100.0F, .offsetX = -10.0F, .offsetY = 30.0F};

/** @brief A hitbox that gives a size of its own beside a box it lists, one
 * whose list is empty, and where and why each is refused. */
constexpr std::string_view HITBOX_OF_A_SIZE_AND_A_BOX =
    "[gameplay.hitbox]\nwidth = 96\nheight = 96\n"
    "[[gameplay.hitbox.box]]\nwidth = 40\nheight = 40\n";
constexpr std::string_view HITBOX_OF_NO_BOX = "[gameplay.hitbox]\nbox = []\n";
constexpr std::string_view HITBOX_WIDTH_DOTTED_KEY = "gameplay.hitbox.width";
constexpr std::string_view HITBOX_BOX_DOTTED_KEY = "gameplay.hitbox.box";
constexpr std::string_view HITBOX_SHAPE_RULE_TEXT =
    "a hitbox gives the size of its one box or lists its boxes, never both";
constexpr std::string_view HITBOX_BOXES_RULE_TEXT =
    "a hitbox that lists its boxes holds at least one";
constexpr std::string_view HITBOX_PARTS_SIDE_RULE_TEXT =
    "only a body of side enemy collides with several boxes";

/** @brief A box of a list with a key no box has, and that key. */
constexpr std::string_view HITBOX_OF_A_BOX_WITH_AN_ATTACK =
    "[[gameplay.hitbox.box]]\nwidth = 40\nheight = 40\nattack = 3\n";
constexpr std::string_view ATTACK_OF_THE_FIRST_BOX_DOTTED_KEY =
    "gameplay.hitbox.box[0].attack";

}  // namespace rtype::game::testing
