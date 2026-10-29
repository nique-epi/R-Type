#include "HelpWalkthrough.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Animation.hpp"
#include "BarState.hpp"
#include "BoundAction.hpp"
#include "BoxCorners.hpp"
#include "Button.hpp"
#include "CutCornerBox.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "HelpPart.hpp"
#include "HelpScene.hpp"
#include "HelpStep.hpp"
#include "HelpWalkthroughConstants.hpp"
#include "HelpWalkthroughParts.hpp"
#include "HelpWord.hpp"
#include "HelpWordLook.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "InterfacePixel.hpp"
#include "KeyBindings.hpp"
#include "KeyCap.hpp"
#include "KeyNames.hpp"
#include "LobbyPartAreas.hpp"
#include "MotionPreference.hpp"
#include "PilotColor.hpp"
#include "PilotStatus.hpp"
#include "PlayerPalette.hpp"
#include "PlayerShading.hpp"
#include "PlayfieldMapping.hpp"
#include "ProfessorSpriteConstants.hpp"
#include "SoundConstants.hpp"
#include "SpriteFrame.hpp"
#include "StatusBand.hpp"
#include "StatusBandConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @returns The names of the keys bound to every action, in the order of
 * BOUND_ACTIONS. */
std::array<std::string, BOUND_ACTION_COUNT> keyNamesOf(
    const KeyBindings& keys, const KeyNaming& keyName) {
  std::array<std::string, BOUND_ACTION_COUNT> names;
  for (std::size_t action = 0; action < names.size(); ++action) {
    names.at(action) = keyName(keys.keyOf(BOUND_ACTIONS.at(action)));
  }
  return names;
}

/** @returns Whether a word is wrapped in the two marks. */
bool isWrapped(std::string_view word, char opening, char closing) {
  return word.size() > 1 && word.front() == opening && word.back() == closing;
}

/** @returns A word wrapped in marks, without them. */
std::string_view insideOf(std::string_view wrapped) {
  return wrapped.substr(1, wrapped.size() - 2);
}

/** @returns The name a keycap shows: the key bound to the action a token of
 * HELP_KEY_TOKENS names, or the word itself. */
std::string keyCapNameOf(std::string_view word,
                         std::span<const std::string> keyNames) {
  for (std::size_t action = 0; action < HELP_KEY_TOKENS.size(); ++action) {
    if (HELP_KEY_TOKENS.at(action) == word) {
      return keyNames[action];
    }
  }
  return std::string(word);
}

/** @returns A word of a sentence as the bubble writes it, the punctuation
 * that ends it apart. */
HelpWord wordOf(std::string_view token, std::span<const std::string> keyNames) {
  const std::size_t coreLength = token.find_last_not_of(HELP_PUNCTUATION) + 1;
  const std::string_view core = token.substr(0, coreLength);
  std::string suffix(token.substr(coreLength));
  if (isWrapped(core, KEY_CAP_OPENING, KEY_CAP_CLOSING)) {
    return {.text = keyCapNameOf(insideOf(core), keyNames),
            .look = HelpWordLook::KeyCap,
            .suffix = std::move(suffix)};
  }
  if (isWrapped(core, INTERFACE_WORD_MARK, INTERFACE_WORD_MARK)) {
    return {.text = std::string(insideOf(core)),
            .look = HelpWordLook::Highlighted,
            .suffix = std::move(suffix)};
  }
  return {
      .text = std::string(token), .look = HelpWordLook::Plain, .suffix = {}};
}

/** @returns The words of a sentence, split on WORD_SEPARATOR. */
std::vector<HelpWord> wordsOf(std::string_view text,
                              std::span<const std::string> keyNames) {
  std::vector<HelpWord> words;
  for (const auto token : std::views::split(text, WORD_SEPARATOR)) {
    if (!token.empty()) {
      words.push_back(
          wordOf(std::string_view(token.begin(), token.end()), keyNames));
    }
  }
  return words;
}

/** @returns How wide a word is written: its text or its keycap, then its
 * punctuation. */
int widthOf(const HelpWord& word) {
  const int text = word.look == HelpWordLook::KeyCap
                       ? KeyCap(word.text, KeyCap::Edge::Plain).width()
                       : regularTextWidth(word.text);
  return text + regularTextWidth(word.suffix);
}

/** @returns The words in lines no wider than HELP_TEXT_WIDTH; a word wider
 * than a line has a line of its own. */
std::vector<std::vector<HelpWord>> linesOf(std::vector<HelpWord> words) {
  std::vector<std::vector<HelpWord>> lines;
  int used = 0;
  for (HelpWord& word : words) {
    const int width = widthOf(word);
    if (lines.empty() || used + HELP_WORD_GAP + width > HELP_TEXT_WIDTH) {
      lines.emplace_back();
      used = -HELP_WORD_GAP;
    }
    used += HELP_WORD_GAP + width;
    lines.back().push_back(std::move(word));
  }
  return lines;
}

/** @returns The lines of every step, the keys named as the player bound
 * them: built once, since the keys do not change while the help lives. */
std::array<std::vector<std::vector<HelpWord>>, HELP_STEPS.size()> stepLinesOf(
    const KeyBindings& keys, const KeyNaming& keyName) {
  const std::array<std::string, BOUND_ACTION_COUNT> keyNames =
      keyNamesOf(keys, keyName);
  std::array<std::vector<std::vector<HelpWord>>, HELP_STEPS.size()> lines;
  for (std::size_t step = 0; step < lines.size(); ++step) {
    lines.at(step) = linesOf(wordsOf(HELP_STEPS.at(step).text, keyNames));
  }
  return lines;
}

/** @returns How tall a bubble of that many lines is: its padding, its lines,
 * then its footer. */
int bubbleHeightOf(std::size_t lineCount) {
  return (SIDES_PER_AXIS * HELP_BUBBLE_PADDING) +
         (static_cast<int>(lineCount) * HELP_LINE_HEIGHT) + HELP_FOOTER_GAP +
         CONTROL_HEIGHT;
}

/** @returns The area grown by a margin on each side. */
sf::IntRect grownBy(sf::IntRect area, int margin) {
  return {area.position - sf::Vector2i(margin, margin),
          area.size +
              sf::Vector2i(SIDES_PER_AXIS * margin, SIDES_PER_AXIS * margin)};
}

/** @returns The part of an area inside the interface picture; the area itself
 * when none of it is. */
sf::IntRect insidePicture(sf::IntRect area) {
  return area.findIntersection(INTERFACE_AREA).value_or(area);
}

bool overlaps(sf::IntRect first, sf::IntRect second) {
  return first.findIntersection(second).has_value();
}

/** @returns Where a part of the room or of the moment of a game lies;
 * nothing for HelpPart::None. */
std::optional<sf::IntRect> partAreaOf(HelpPart part, const LobbyPartAreas& room,
                                      sf::IntRect chip) {
  switch (part) {
    case HelpPart::None:
      return std::nullopt;
    case HelpPart::Bays:
      return room.bays;
    case HelpPart::OwnBay:
      return room.ownBay;
    case HelpPart::Ready:
      return room.ready;
    case HelpPart::Status:
      return room.status;
    case HelpPart::Back:
      return room.back;
    case HelpPart::Help:
      return chip;
    case HelpPart::Ship:
      return HELP_SHIP_PART;
    case HelpPart::Shots:
      return HELP_SHOTS_PART;
    case HelpPart::Bydos:
      return HELP_BYDOS_PART;
    case HelpPart::EnemyShots:
      return HELP_ENEMY_SHOTS_PART;
    case HelpPart::OwnBand:
      return HELP_OWN_BAND_PART;
  }
  return std::nullopt;
}

/** @returns Where the bubble goes: low unless the part would come near it,
 * then high unless the part lies under the high bubble too; in the middle
 * for no part. */
sf::IntRect bubbleAreaOf(HelpScene scene, std::optional<sf::IntRect> part,
                         int height) {
  const int left = (INTERFACE_AREA.size.x - HELP_BUBBLE_WIDTH) / SIDES_PER_AXIS;
  const sf::Vector2i size(HELP_BUBBLE_WIDTH, height);
  if (!part.has_value()) {
    return {{left, ((INTERFACE_AREA.size.y - height) / SIDES_PER_AXIS) +
                       HELP_MIDDLE_DROP},
            size};
  }
  const int lowBottom = scene == HelpScene::Game
                            ? STATUS_BAND_AREA.position.y - HELP_BAND_CLEARANCE
                            : INTERFACE_AREA.size.y - SAFE_MARGIN;
  const sf::IntRect low({left, lowBottom - height}, size);
  const sf::IntRect high(
      {left, scene == HelpScene::Game ? HELP_TOP_IN_GAME : HELP_TOP_IN_ROOM},
      size);
  const sf::IntRect near = grownBy(*part, HELP_PART_CLEARANCE);
  if (!overlaps(near, grownBy(low, HELP_PROFESSOR_ROOM)) ||
      overlaps(near, high)) {
    return low;
  }
  return high;
}

/** @returns Where the professor stands: beside the bubble, on the side of
 * the part, level with the bottom of a low bubble or the top of a high one;
 * on the other side when it would cover the part. */
sf::Vector2i professorTopLeftOf(sf::IntRect bubble,
                                std::optional<sf::IntRect> part) {
  const sf::Vector2i size = HELP_PROFESSOR_SIZE;
  const bool high =
      bubble.getCenter().y < INTERFACE_AREA.size.y / SIDES_PER_AXIS;
  const int top =
      high ? bubble.position.y : bubble.position.y + bubble.size.y - size.y;
  const sf::IntRect left({bubble.position.x - HELP_PROFESSOR_GAP - size.x, top},
                         size);
  const sf::IntRect right(
      {bubble.position.x + bubble.size.x + HELP_PROFESSOR_GAP, top}, size);
  if (!part.has_value()) {
    return left.position;
  }
  const bool partOnTheRight =
      part->getCenter().x > INTERFACE_AREA.size.x / SIDES_PER_AXIS;
  const sf::IntRect first = partOnTheRight ? right : left;
  const sf::IntRect other = partOnTheRight ? left : right;
  return overlaps(first, grownBy(*part, HELP_PROFESSOR_CLEARANCE))
             ? other.position
             : first.position;
}

/** @returns The frame an animation shows at that time; its first frame when it
 * shows none. */
game::SpriteFrame frameOf(const Animation& animation, engine::Duration time) {
  return animation.frameAt(time).value_or(animation.frames.front());
}

/** @returns The frame of the professor standing there: facing the part, at
 * that time. */
game::SpriteFrame professorFrameOf(sf::Vector2i topLeft,
                                   std::optional<sf::IntRect> part,
                                   engine::Duration time) {
  const bool facesRight =
      part.has_value() &&
      part->getCenter().x >
          sf::IntRect(topLeft, HELP_PROFESSOR_SIZE).getCenter().x;
  return frameOf(facesRight ? PROFESSOR_FACING_RIGHT_ANIMATION
                            : PROFESSOR_FACING_LEFT_ANIMATION,
                 time);
}

/** @returns How far out the corners stand at that time: HELP_CORNER_BREATH
 * in one phase of HELP_CORNER_BREATH_PHASES, none in the others. */
int breathAt(engine::Duration time) {
  return (time / HELP_CORNER_BREATH_TIME) % HELP_CORNER_BREATH_PHASES == 1
             ? HELP_CORNER_BREATH
             : 0;
}

/** @brief Dims the picture with the layer of dialogs, but for the part grown
 * by HELP_SPOTLIGHT_GAP, then frames it with four corners; dims all of the
 * picture for no part. */
void paintSpotlight(IInterfaceCanvas& canvas, std::optional<sf::IntRect> part,
                    engine::Duration time) {
  if (!part.has_value()) {
    canvas.fill(INTERFACE_AREA, DIM_LAYER_COLOR);
    return;
  }
  const sf::IntRect hole = insidePicture(grownBy(*part, HELP_SPOTLIGHT_GAP));
  const int holeRight = hole.position.x + hole.size.x;
  const int holeBottom = hole.position.y + hole.size.y;
  canvas.fill({{0, 0}, {INTERFACE_AREA.size.x, hole.position.y}},
              DIM_LAYER_COLOR);
  canvas.fill({{0, holeBottom},
               {INTERFACE_AREA.size.x, INTERFACE_AREA.size.y - holeBottom}},
              DIM_LAYER_COLOR);
  canvas.fill({{0, hole.position.y}, {hole.position.x, hole.size.y}},
              DIM_LAYER_COLOR);
  canvas.fill({{holeRight, hole.position.y},
               {INTERFACE_AREA.size.x - holeRight, hole.size.y}},
              DIM_LAYER_COLOR);
  paintBoxCorners(
      canvas,
      insidePicture(grownBy(*part, HELP_SPOTLIGHT_GAP + breathAt(time))),
      HELP_CORNER_COLOR);
}

/** @brief Writes a word at its place on a line, then its punctuation.
 * @returns How wide it was written. */
int paintWord(IInterfaceCanvas& canvas, const HelpWord& word,
              sf::Vector2i topLeft) {
  int width = 0;
  if (word.look == HelpWordLook::KeyCap) {
    const KeyCap keyCap(word.text, KeyCap::Edge::Plain);
    keyCap.paint(canvas, topLeft - sf::Vector2i(0, CONTROL_TEXT_TOP));
    width = keyCap.width();
  } else {
    canvas.write(word.text, topLeft,
                 word.look == HelpWordLook::Highlighted ? DIALOG_HEADING_STYLE
                                                        : DIALOG_MESSAGE_STYLE);
    width = regularTextWidth(word.text);
  }
  if (!word.suffix.empty()) {
    canvas.write(word.suffix, topLeft + sf::Vector2i(width, 0),
                 DIALOG_MESSAGE_STYLE);
  }
  return width + regularTextWidth(word.suffix);
}

/** @brief Writes the lines of the bubble, HELP_BUBBLE_PADDING in from its
 * top-left corner. */
void paintLines(IInterfaceCanvas& canvas, sf::IntRect bubble,
                const std::vector<std::vector<HelpWord>>& lines) {
  sf::Vector2i lineTopLeft =
      bubble.position + sf::Vector2i(HELP_BUBBLE_PADDING, HELP_BUBBLE_PADDING);
  for (const std::vector<HelpWord>& line : lines) {
    sf::Vector2i wordTopLeft = lineTopLeft;
    for (const HelpWord& word : line) {
      wordTopLeft.x += paintWord(canvas, word, wordTopLeft) + HELP_WORD_GAP;
    }
    lineTopLeft.y += HELP_LINE_HEIGHT;
  }
}

sf::Color pipColorOf(std::size_t pip, std::size_t step) {
  if (pip < step) {
    return SEEN_PIP_COLOR;
  }
  return pip == step ? CURRENT_PIP_COLOR : COMING_PIP_COLOR;
}

/** @brief Paints the progress at the left of the footer: one pip per step,
 * then the number of the step out of their count, so the progress never rests
 * on color alone. */
void paintProgress(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                   std::size_t step) {
  for (std::size_t pip = 0; pip < HELP_STEPS.size(); ++pip) {
    canvas.fill({topLeft + sf::Vector2i(static_cast<int>(pip) * HELP_PIP_STEP,
                                        HELP_PIP_TOP),
                 {HELP_PIP_SIZE, HELP_PIP_SIZE}},
                pipColorOf(pip, step));
  }
  const int pipsWidth = static_cast<int>(HELP_STEPS.size()) * HELP_PIP_STEP;
  canvas.write(
      std::to_string(step + 1) + HELP_COUNT_SEPARATOR +
          std::to_string(HELP_STEPS.size()),
      topLeft + sf::Vector2i(pipsWidth + HELP_COUNT_GAP, CONTROL_TEXT_TOP),
      HELP_FOOTER_STYLE);
}

/** @brief Paints a keycap and the word of its key, the word ending at the
 * right of the area. @returns Where the keycap starts. */
int paintKeyHint(IInterfaceCanvas& canvas, sf::Vector2i topRight,
                 std::pair<std::string_view, std::string_view> keyAndWord) {
  const int wordLeft =
      topRight.x - canvas.textWidth(keyAndWord.second, TextFont::Small);
  canvas.write(keyAndWord.second, {wordLeft, topRight.y + CONTROL_TEXT_TOP},
               HELP_FOOTER_STYLE);
  const KeyCap keyCap(std::string(keyAndWord.first), KeyCap::Edge::Plain);
  const int keyCapLeft = wordLeft - HELP_KEY_WORD_GAP - keyCap.width();
  keyCap.paint(canvas, {keyCapLeft, topRight.y});
  return keyCapLeft;
}

/** @brief Paints the footer of the bubble: a rule, the progress, then ENTER
 * NEXT, or ENTER CLOSE on the last step, and ESC SKIP before it otherwise. */
void paintFooter(IInterfaceCanvas& canvas, sf::IntRect bubble,
                 std::size_t step) {
  const int top =
      bubble.position.y + bubble.size.y - HELP_BUBBLE_PADDING - CONTROL_HEIGHT;
  const int left = bubble.position.x + HELP_BUBBLE_PADDING;
  const int right = bubble.position.x + bubble.size.x - HELP_BUBBLE_PADDING;
  canvas.fill({{left, top - HELP_RULE_DROP}, {right - left, BORDER_THICKNESS}},
              HELP_RULE_COLOR);
  paintProgress(canvas, {left, top}, step);
  const bool lastStep = step + 1 == HELP_STEPS.size();
  const int nextLeft =
      paintKeyHint(canvas, {right, top},
                   {CONFIRM_KEY_NAME, lastStep ? CLOSE_WORD : NEXT_WORD});
  if (!lastStep) {
    paintKeyHint(canvas, {nextLeft - HELP_FOOTER_GROUP_GAP, top},
                 {CANCEL_KEY_NAME, SKIP_WORD});
  }
}

/** @brief Draws a frame of a sheet at that place, anywhere on the picture. */
void paintFrame(IInterfaceCanvas& canvas, TextureArea frame,
                ImagePlacement placement) {
  canvas.drawImage(frame, placement, INTERFACE_AREA);
}

TextureArea entityFrame(game::SpriteFrame frame) {
  return {.assetId = ENTITY_SHEET_ID, .rectangle = rectangleOf(frame)};
}

/** @returns The pilots of the status band of the moment of a game, in the
 * order of the player colors, the local player first. */
std::array<PilotStatus, HELP_PILOT_NICKNAMES.size()> stagedPilots() {
  std::array<PilotStatus, HELP_PILOT_NICKNAMES.size()> pilots;
  for (std::size_t color = 0; color < pilots.size(); ++color) {
    pilots.at(color) = {.playerNumber = static_cast<std::uint8_t>(
                            playerNumberOfPilotColor(static_cast<int>(color))),
                        .nickname = std::string(HELP_PILOT_NICKNAMES.at(color)),
                        .score = HELP_PILOT_SCORES.at(color),
                        .lives = HELP_PILOT_LIVES.at(color),
                        .state = PilotState::Flying,
                        .isLocalPlayer = color == HELP_OWN_COLOR};
  }
  return pilots;
}

/** @brief Paints the moment of a game: the ships, the missiles, the Bydos and
 * their missiles at that time, then the status band. */
void paintGameScene(IInterfaceCanvas& canvas, const PlayerPalette& palette,
                    engine::Duration time) {
  for (std::size_t color = 0; color < HELP_SHIP_TOP_LEFTS.size(); ++color) {
    paintFrame(
        canvas,
        entityFrame(inPlayerColorRow(frameOf(PLAYER_SHIP_ANIMATION, time),
                                     static_cast<int>(color))),
        {.topLeft = HELP_SHIP_TOP_LEFTS.at(color),
         .recolor = shadingOf(palette, color).recolor});
  }
  for (const sf::Vector2i topLeft : HELP_MISSILE_TOP_LEFTS) {
    paintFrame(canvas, entityFrame(frameOf(PLAYER_MISSILE_ANIMATION, time)),
               {.topLeft = topLeft});
  }
  for (const sf::Vector2i topLeft : HELP_BYDO_TOP_LEFTS) {
    paintFrame(canvas, entityFrame(frameOf(BYDO_ANIMATION, time)),
               {.topLeft = topLeft});
  }
  for (const sf::Vector2i topLeft : HELP_ENEMY_MISSILE_TOP_LEFTS) {
    paintFrame(canvas, entityFrame(frameOf(ENEMY_MISSILE_ANIMATION, time)),
               {.topLeft = topLeft});
  }
  paintStatusBand(canvas, stagedPilots(), palette);
}

/** @returns The step an event asks an open help for, on that step: the next
 * one, none after the last, for a key of HELP_NEXT_KEYS or a left click; the
 * previous one, still the first on the first, for a key of
 * HELP_PREVIOUS_KEYS; none for MENU_CANCEL_KEY; that step for anything
 * else. */
std::optional<std::size_t> stepAskedBy(const sf::Event& event,
                                       std::size_t step) {
  const std::optional<std::size_t> next =
      step + 1 < HELP_STEPS.size() ? std::optional(step + 1) : std::nullopt;
  if (const auto* clicked = event.getIf<sf::Event::MouseButtonPressed>()) {
    return clicked->button == sf::Mouse::Button::Left ? next : step;
  }
  const auto* pressed = event.getIf<sf::Event::KeyPressed>();
  if (pressed == nullptr) {
    return step;
  }
  if (std::ranges::find(HELP_NEXT_KEYS, pressed->code) !=
      HELP_NEXT_KEYS.end()) {
    return next;
  }
  if (std::ranges::find(HELP_PREVIOUS_KEYS, pressed->code) !=
      HELP_PREVIOUS_KEYS.end()) {
    return step == 0 ? step : step - 1;
  }
  return pressed->code == MENU_CANCEL_KEY ? std::nullopt : std::optional(step);
}

/** @returns Whether an event ends an input the room may have started before
 * the help opened: the release of a key or of a button, or the focus lost. */
bool endsAnInput(const sf::Event& event) {
  return event.is<sf::Event::KeyReleased>() ||
         event.is<sf::Event::MouseButtonReleased>() ||
         event.is<sf::Event::FocusLost>();
}

}  // namespace

HelpWalkthrough::HelpWalkthrough(const HelpWalkthroughParts& parts)
    : sounds_(&parts.sounds.get()),
      stepLines_(stepLinesOf(parts.keys.get(), parts.keyName)),
      palette_(parts.palette),
      motion_(parts.motion),
      chipArea_(parts.chipArea),
      chip_(Button::Text{.label = HELP_LABEL, .keyName = HELP_KEY_NAME},
            parts.chipArea, nullptr, std::nullopt) {}

bool HelpWalkthrough::handleEvent(const sf::Event& event,
                                  const PlayfieldMapping& playfieldPointAt) {
  if (step_.has_value()) {
    followStep(event);
    return !endsAnInput(event);
  }
  if (isAskedToOpen(event, playfieldPointAt)) {
    show(0);
    return true;
  }
  return false;
}

void HelpWalkthrough::advance(engine::Duration elapsed) { elapsed_ += elapsed; }

bool HelpWalkthrough::showsGame() const {
  return step_.has_value() && HELP_STEPS.at(*step_).scene == HelpScene::Game;
}

void HelpWalkthrough::paint(IInterfaceCanvas& canvas,
                            const LobbyPartAreas& room) const {
  if (showsGame()) {
    paintGameScene(canvas, palette_, shownTime());
  } else {
    chip_.paint(canvas, BarState::Idle);
  }
  if (step_.has_value()) {
    paintStep(canvas, room, *step_);
  }
}

bool HelpWalkthrough::isAskedToOpen(
    const sf::Event& event, const PlayfieldMapping& playfieldPointAt) const {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    return pressed->code == HELP_KEY;
  }
  const auto* clicked = event.getIf<sf::Event::MouseButtonPressed>();
  return clicked != nullptr && clicked->button == sf::Mouse::Button::Left &&
         chip_.contains(interfacePixelAt(playfieldPointAt(clicked->position)));
}

void HelpWalkthrough::followStep(const sf::Event& event) {
  const std::optional<std::size_t> asked =
      stepAskedBy(event, step_.value_or(0));
  if (asked != step_) {
    show(asked);
  }
}

void HelpWalkthrough::show(std::optional<std::size_t> step) {
  step_ = step;
  sounds_->play(CLICK_SOUND_ID);
}

void HelpWalkthrough::paintStep(IInterfaceCanvas& canvas,
                                const LobbyPartAreas& room,
                                std::size_t step) const {
  const HelpStep& shown = HELP_STEPS.at(step);
  const std::optional<sf::IntRect> part =
      partAreaOf(shown.part, room, chipArea_);
  const std::vector<std::vector<HelpWord>>& lines = stepLines_.at(step);
  const sf::IntRect bubble =
      bubbleAreaOf(shown.scene, part, bubbleHeightOf(lines.size()));
  const sf::Vector2i professor = professorTopLeftOf(bubble, part);
  paintSpotlight(canvas, part, shownTime());
  CutCornerBox(bubble,
               {.border = PANEL_BORDER_COLOR, .inside = PANEL_INSIDE_COLOR})
      .paint(canvas);
  paintLines(canvas, bubble, lines);
  paintFooter(canvas, bubble, step);
  paintFrame(canvas,
             {.assetId = PROFESSOR_SHEET_ID,
              .rectangle =
                  rectangleOf(professorFrameOf(professor, part, shownTime()))},
             {.topLeft = professor, .scale = HELP_PROFESSOR_SCALE});
}

engine::Duration HelpWalkthrough::shownTime() const {
  return motion_ == MotionPreference::Full ? elapsed_ : engine::Duration{};
}

}  // namespace rtype::client
