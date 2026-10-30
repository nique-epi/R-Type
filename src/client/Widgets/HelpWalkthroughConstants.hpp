#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "BoundAction.hpp"
#include "EntitySpriteConstants.hpp"
#include "HelpPart.hpp"
#include "HelpScene.hpp"
#include "HelpStep.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "ProfessorSpriteConstants.hpp"
#include "StatusBandConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief The chip that opens the help, beside BACK, and its key: a fixed
 * key, like MENU_CANCEL_KEY. A binding the player gives the same key loses to
 * the help in the waiting room, which hands the help every event first. */
constexpr const char* HELP_LABEL = "HELP";
constexpr sf::Keyboard::Key HELP_KEY = sf::Keyboard::Key::H;
constexpr const char* HELP_KEY_NAME = "H";

/** @brief Keys that show the next step, the last one closing the help, and
 * keys that show the previous one; MENU_CANCEL_KEY closes the help from any
 * step. */
constexpr std::array HELP_NEXT_KEYS{MENU_CONFIRM_KEY, sf::Keyboard::Key::Space,
                                    sf::Keyboard::Key::Right};
constexpr std::array HELP_PREVIOUS_KEYS{sf::Keyboard::Key::Left,
                                        sf::Keyboard::Key::Backspace};

/** @brief The words between braces that name the key bound to an action, in
 * the order of BOUND_ACTIONS. */
constexpr std::array<std::string_view, BOUND_ACTION_COUNT> HELP_KEY_TOKENS{
    "UP", "DOWN", "LEFT", "RIGHT", "SHOOT", "READY"};

/** @brief Marks of a sentence: a keycap between braces, a word of the
 * interface between stars, the punctuation written plain after either, and
 * the separator of the words. */
constexpr char KEY_CAP_OPENING = '{';
constexpr char KEY_CAP_CLOSING = '}';
constexpr char INTERFACE_WORD_MARK = '*';
constexpr std::string_view HELP_PUNCTUATION = ".,:!?";
constexpr char WORD_SEPARATOR = ' ';

/** @brief The steps of the help, in order: the parts of the waiting room, a
 * moment of a game, then the HELP chip again. */
constexpr std::array HELP_STEPS{
    HelpStep{.scene = HelpScene::Lobby,
             .part = HelpPart::None,
             .text = "HELLO, PILOT! I AM PROFESSOR BYDO. I WILL SHOW YOU THE "
                     "*WAITING* *ROOM*, THEN A GAME."},
    HelpStep{.scene = HelpScene::Lobby,
             .part = HelpPart::Bays,
             .text = "EACH *BAY* IS ONE PILOT. UP TO 4 PILOTS FLY TOGETHER."},
    HelpStep{.scene = HelpScene::Lobby,
             .part = HelpPart::OwnBay,
             .text = "THIS BAY SAYS *YOU*: IT IS YOURS. YOUR SHIP HAS YOUR "
                     "COLOR."},
    HelpStep{.scene = HelpScene::Lobby,
             .part = HelpPart::Ready,
             .text = "PRESS {READY} OR *READY* WHEN YOU ARE READY TO FLY."},
    HelpStep{.scene = HelpScene::Lobby,
             .part = HelpPart::Status,
             .text = "THIS LINE SAYS WHO IS *NOT* *READY*. THE GAME STARTS "
                     "WHEN ALL ARE READY."},
    HelpStep{.scene = HelpScene::Lobby,
             .part = HelpPart::Back,
             .text = "{ESC} OR *BACK* LEAVES THE SERVER."},
    HelpStep{.scene = HelpScene::Lobby,
             .part = HelpPart::Help,
             .text = "{H} OR *HELP* SHOWS ME AGAIN."},
    HelpStep{.scene = HelpScene::Game,
             .part = HelpPart::None,
             .text = "NOW, A GAME! YOU FLY TO THE RIGHT. THE BYDOS COME TO "
                     "MEET YOU."},
    HelpStep{.scene = HelpScene::Game,
             .part = HelpPart::Ship,
             .text = "THIS IS *YOUR* *SHIP*. MOVE IT WITH {UP} {DOWN} {LEFT} "
                     "{RIGHT}."},
    HelpStep{.scene = HelpScene::Game,
             .part = HelpPart::Shots,
             .text = "PRESS {SHOOT} TO *SHOOT*. HOLD IT TO KEEP SHOOTING."},
    HelpStep{.scene = HelpScene::Game,
             .part = HelpPart::Bydos,
             .text = "SHOOT THE *BYDOS* FOR POINTS. NOT ME, PLEASE!"},
    HelpStep{.scene = HelpScene::Game,
             .part = HelpPart::EnemyShots,
             .text = "DODGE THEIR *ROUND* *MISSILES*. A HIT COSTS ONE LIFE."},
    HelpStep{.scene = HelpScene::Game,
             .part = HelpPart::OwnBand,
             .text = "YOUR *LIVES* AND *SCORE* ARE HERE. *OUT* MEANS NO LIFE "
                     "LEFT."},
    HelpStep{.scene = HelpScene::Game,
             .part = HelpPart::None,
             .text = "THE GOAL: STAY ALIVE AND SHOOT BYDOS. THE BEST SCORE "
                     "WINS."},
    HelpStep{.scene = HelpScene::Lobby,
             .part = HelpPart::Help,
             .text = "THAT IS ALL! {H} BRINGS ME BACK ANY TIME."},
};

/**
 * @brief The bubble: HELP_BUBBLE_WIDTH wide and centered, its text
 * HELP_BUBBLE_PADDING in, a line every HELP_LINE_HEIGHT pixels so a keycap
 * fits, HELP_WORD_GAP between two words; its footer, a keycap tall,
 * HELP_FOOTER_GAP under the text.
 */
constexpr int HELP_BUBBLE_WIDTH = 304;
constexpr int HELP_BUBBLE_PADDING = 8;
constexpr int HELP_LINE_HEIGHT = 13;
constexpr int HELP_WORD_GAP = REGULAR_FONT_ADVANCE;
constexpr int HELP_FOOTER_GAP = 6;
constexpr int HELP_TEXT_WIDTH =
    HELP_BUBBLE_WIDTH - (SIDES_PER_AXIS * HELP_BUBBLE_PADDING);

/**
 * @brief Where the bubble goes: low by default, SAFE_MARGIN above the bottom
 * of the room or HELP_BAND_CLEARANCE above the status band of a game; high,
 * at the top of the bays or at HELP_TOP_IN_GAME, when the part it explains,
 * grown by HELP_PART_CLEARANCE, would touch the low bubble grown by
 * HELP_PROFESSOR_ROOM, unless the part lies under the high bubble too; in the
 * middle, HELP_MIDDLE_DROP lower, for a step that lights no part.
 */
constexpr int HELP_BAND_CLEARANCE = 8;
constexpr int HELP_TOP_IN_ROOM = ROOM_ILLUSTRATION_TOP;
constexpr int HELP_TOP_IN_GAME = 40;
constexpr int HELP_PART_CLEARANCE = 10;
constexpr int HELP_PROFESSOR_ROOM = 26;
constexpr int HELP_MIDDLE_DROP = 18;

/** @brief The professor, drawn HELP_PROFESSOR_SCALE times, stands
 * HELP_PROFESSOR_GAP from the bubble, and keeps HELP_PROFESSOR_CLEARANCE from
 * the part it explains. */
constexpr int HELP_PROFESSOR_SCALE = 2;
constexpr int HELP_PROFESSOR_GAP = 2;
constexpr int HELP_PROFESSOR_CLEARANCE = 4;
constexpr sf::Vector2i HELP_PROFESSOR_SIZE =
    sf::Vector2i(PROFESSOR_FRAME.width, PROFESSOR_FRAME.height) *
    HELP_PROFESSOR_SCALE;

/**
 * @brief The footer: a rule HELP_RULE_DROP above it; at its left, one pip
 * per step, HELP_PIP_SIZE square every HELP_PIP_STEP pixels, then the number
 * of the step out of their count, HELP_COUNT_GAP after the pips; at its right
 * end ESC SKIP then ENTER NEXT, CLOSE on the last step, each word
 * HELP_KEY_WORD_GAP after its keycap, the two HELP_FOOTER_GROUP_GAP apart.
 */
constexpr int HELP_RULE_DROP = 3;
constexpr int HELP_PIP_SIZE = 3;
constexpr int HELP_PIP_STEP = 4;
constexpr int HELP_PIP_TOP = 4;
constexpr int HELP_COUNT_GAP = 4;
constexpr int HELP_KEY_WORD_GAP = 3;
constexpr int HELP_FOOTER_GROUP_GAP = 10;
constexpr const char* HELP_COUNT_SEPARATOR = "/";
constexpr std::string_view NEXT_WORD = "NEXT";
constexpr std::string_view CLOSE_WORD = "CLOSE";
constexpr std::string_view SKIP_WORD = "SKIP";

/** @brief Colors of the help, its sentences being written in the styles of
 * a dialog: its footer and pips, and the corners of the lit part. */
constexpr TextStyle HELP_FOOTER_STYLE{.font = TextFont::Small,
                                      .color = PALE_BLUE};
constexpr sf::Color HELP_RULE_COLOR = DEEP_BLUE;
constexpr sf::Color SEEN_PIP_COLOR = EDGE_BLUE;
constexpr sf::Color CURRENT_PIP_COLOR = HUD_CYAN;
constexpr sf::Color COMING_PIP_COLOR = DEEP_BLUE;
constexpr sf::Color HELP_CORNER_COLOR = HUD_CYAN;

/** @brief The dim layer stops HELP_SPOTLIGHT_GAP pixels from the lit part,
 * and its corners stand as far out, HELP_CORNER_BREATH further one
 * HELP_CORNER_BREATH_TIME in HELP_CORNER_BREATH_PHASES while motion is
 * allowed. */
constexpr int HELP_SPOTLIGHT_GAP = 3;
constexpr int HELP_CORNER_BREATH = 1;
constexpr int HELP_CORNER_BREATH_PHASES = 2;
constexpr engine::Duration HELP_CORNER_BREATH_TIME =
    std::chrono::milliseconds(400);

/**
 * @brief The moment of a game, as on the picture of the help the team chose:
 * the ships of the first three player colors, the local player flying the
 * first; the missiles of the local player and of the second ship; three
 * Bydos and two of their missiles.
 */
constexpr std::size_t HELP_OWN_COLOR = 0;
constexpr std::array HELP_SHIP_TOP_LEFTS{
    sf::Vector2i(96, 104), sf::Vector2i(64, 168), sf::Vector2i(140, 60)};
constexpr std::array HELP_MISSILE_TOP_LEFTS{
    sf::Vector2i(150, 109), sf::Vector2i(200, 109), sf::Vector2i(104, 173)};
constexpr std::array HELP_BYDO_TOP_LEFTS{
    sf::Vector2i(330, 84), sf::Vector2i(388, 130), sf::Vector2i(310, 168)};
constexpr std::array HELP_ENEMY_MISSILE_TOP_LEFTS{sf::Vector2i(286, 132),
                                                  sf::Vector2i(262, 146)};

/** @brief The parts of that moment the help lights: the ship of the local
 * player, its two missiles, the Bydos, their missiles, and the bay of the
 * local player in the status band. */
constexpr sf::IntRect HELP_SHIP_PART(HELP_SHIP_TOP_LEFTS.at(HELP_OWN_COLOR),
                                     {PLAYER_SHIP_FRAME.width,
                                      PLAYER_SHIP_FRAME.height});
constexpr sf::IntRect HELP_SHOTS_PART({150, 109}, {66, 6});
constexpr sf::IntRect HELP_BYDOS_PART({310, 84}, {102, 108});
constexpr sf::IntRect HELP_ENEMY_SHOTS_PART({262, 132}, {32, 22});
constexpr sf::IntRect HELP_OWN_BAND_PART({STATUS_BAY_INSET,
                                          STATUS_BAND_AREA.position.y},
                                         {STATUS_BAY_WIDTH, STATUS_BAND_ROWS});

/** @brief The pilots of its status band, in the order of the player colors:
 * nickname, lives and score, the last on its last life. */
constexpr std::array<std::string_view, 3> HELP_PILOT_NICKNAMES{"ACE", "NOVA",
                                                               "ZED"};
constexpr std::array<std::uint8_t, HELP_PILOT_NICKNAMES.size()>
    HELP_PILOT_LIVES{3, 2, 1};
constexpr std::array<std::uint32_t, HELP_PILOT_NICKNAMES.size()>
    HELP_PILOT_SCORES{1200, 800, 300};

}  // namespace rtype::client
