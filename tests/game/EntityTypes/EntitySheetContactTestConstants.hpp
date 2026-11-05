#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::game::testing {

/** @brief Pieces of `[gameplay]` that decide what a contact with a ship does.
 * A hurt zone is an array of tables `[[gameplay.hurt_zone.box]]`; the first
 * box of TWO_HURT_BOXES hurts all the time, the second only in the strike of
 * attack 1. */
constexpr std::string_view TWO_HURT_BOXES =
    "[[gameplay.hurt_zone.box]]\noffset_x = 48\noffset_y = -8\nwidth = 64\n"
    "height = 32\n\n"
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 16\nwidth = 40\n"
    "height = 24\nattack = 1\nphase = \"strike\"\n";
constexpr std::string_view EVERY_PHASE_OF_ATTACK_TWO =
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\nattack = 2\nphase = \"signal\"\n\n"
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\nattack = 2\nphase = \"strike\"\n\n"
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\nattack = 2\nphase = \"recover\"\n\n"
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\nattack = 2\nphase = \"enraged\"\n";
constexpr std::string_view HURT_ZONE_WITHOUT_BOX = "[gameplay.hurt_zone]\n";
constexpr std::string_view HURT_ZONE_WITH_AN_EMPTY_ARRAY =
    "[gameplay.hurt_zone]\nbox = []\n";
constexpr std::string_view HURT_ZONE_WITH_A_LONE_TABLE =
    "[gameplay.hurt_zone.box]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\n";
constexpr std::string_view BOX_WITH_AN_ATTACK_ONLY =
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\nattack = 1\n";
constexpr std::string_view BOX_WITH_A_PHASE_ONLY =
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\nphase = \"strike\"\n";
constexpr std::string_view BOX_OF_AN_UNKNOWN_PHASE =
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\nattack = 1\nphase = \"stike\"\n";
constexpr std::string_view BOX_WITH_AN_UNKNOWN_KEY =
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\ndamage = 3\n";
constexpr std::string_view BOX_WITHOUT_HEIGHT =
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n";
constexpr std::string_view SECOND_BOX_TOO_WIDE =
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 8\n"
    "height = 8\n\n"
    "[[gameplay.hurt_zone.box]]\noffset_x = 0\noffset_y = 0\nwidth = 1025\n"
    "height = 8\n";
constexpr std::string_view SURVIVES_CONTACT_MARKER =
    "[gameplay.survives_contact]\n";
constexpr std::string_view SURVIVES_CONTACT_WITH_A_KEY =
    "[gameplay.survives_contact]\nsurvives = true\n";

/** @brief What the first box of TWO_HURT_BOXES holds. */
constexpr float FIRST_BOX_OFFSET_X = 48.0F;
constexpr float FIRST_BOX_OFFSET_Y = -8.0F;
constexpr float FIRST_BOX_WIDTH = 64.0F;
constexpr float FIRST_BOX_HEIGHT = 32.0F;

/** @brief What the second box of TWO_HURT_BOXES holds. */
constexpr float SECOND_BOX_OFFSET_Y = 16.0F;
constexpr std::uint8_t SECOND_BOX_ATTACK = 1;
constexpr std::size_t HURT_BOXES_IN_TWO = 2;
constexpr std::size_t HURT_BOXES_IN_EVERY_PHASE = 4;
constexpr std::uint8_t ATTACK_OF_EVERY_PHASE = 2;

/** @brief What the refusals say. */
constexpr std::string_view EMPTY_ZONE_RULE_TEXT =
    "a hurt zone holds at least one box";
constexpr std::string_view ACTION_TOGETHER_RULE_TEXT =
    "a box names the attack and the phase it hurts in together, or neither";
constexpr std::string_view HURT_ZONE_SIDE_RULE_TEXT =
    "only a body of side enemy has a hurt zone";
constexpr std::string_view SURVIVES_CONTACT_SIDE_RULE_TEXT =
    "only a body of side enemy survives a contact";
constexpr std::string_view ATTACK_DOTTED_KEY =
    "gameplay.hurt_zone.box[0].attack";
constexpr std::string_view PHASE_DOTTED_KEY = "gameplay.hurt_zone.box[0].phase";
constexpr std::string_view SECOND_WIDTH_DOTTED_KEY =
    "gameplay.hurt_zone.box[1].width";
constexpr std::string_view HURT_ZONE_DOTTED_KEY = "gameplay.hurt_zone";

}  // namespace rtype::game::testing
