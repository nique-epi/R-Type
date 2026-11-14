#include "SpokenLine.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "SaidLine.hpp"
#include "SpeechLines.hpp"
#include "SpeechLinesConstants.hpp"
#include "SpokenLineConstants.hpp"
#include "SpokenLineParts.hpp"
#include "TimeConstants.hpp"
#include "Typing.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How tall a bubble of @p lineCount lines is: its padding around
 * its lines, the last one a line of text tall. */
int bubbleHeightOf(std::size_t lineCount) {
  return (SIDES_PER_AXIS * SPOKEN_BUBBLE_PADDING) +
         ((static_cast<int>(lineCount) - 1) * SPEECH_LINE_HEIGHT) +
         TEXT_LINE_HEIGHT;
}

/** @brief Paints the tail of a bubble, its middle column at @p top.x, from
 * the row under the bubble down: in the color of the border, its inside
 * opening through the bottom edge of the bubble. */
void paintTail(IInterfaceCanvas& canvas, sf::Vector2i top) {
  for (int row = 0; row < SPOKEN_TAIL_ROWS; ++row) {
    const int half = SPOKEN_TAIL_HALF_WIDTH - row;
    canvas.fill({{top.x - half, top.y + row}, {(SIDES_PER_AXIS * half) + 1, 1}},
                PANEL_BORDER_COLOR);
    if (half > 0) {
      canvas.fill({{top.x - half + BORDER_THICKNESS, top.y + row - 1},
                   {(SIDES_PER_AXIS * half) - 1, 1}},
                  PANEL_INSIDE_COLOR);
    }
  }
}

/** @returns The line @p text, cut to @p width, nothing of it said yet; under
 * less motion only its first word is voiced, since it shows whole at once. */
SaidLine saidLineOf(std::string_view text, int width, MotionPreference motion) {
  SpeechLines lines = speechLinesOf(text, {}, width);
  std::vector<std::size_t> wordStarts = speechWordStartsOf(lines);
  if (motion == MotionPreference::Reduced && wordStarts.size() > 1) {
    wordStarts.resize(1);
  }
  const std::size_t letters = speechLetterCountOf(lines);
  return {.lines = std::move(lines),
          .letters = letters,
          .wordStarts = std::move(wordStarts),
          .voicedWords = 0,
          .saidFor = {}};
}

/** @returns How many letters of @p said show now: all of them under less
 * motion. */
std::size_t lettersShownOf(const SaidLine& said, MotionPreference motion) {
  if (motion == MotionPreference::Reduced) {
    return said.letters;
  }
  return lettersTypedAt(said.saidFor, SPOKEN_LETTER_TIME, said.letters);
}

/** @returns How long @p said shows in all: its writing, none under less
 * motion, then its lingering. */
engine::Duration shownTimeOf(const SaidLine& said, MotionPreference motion) {
  const engine::Duration writing =
      motion == MotionPreference::Full
          ? typingTimeOf(SPOKEN_LETTER_TIME, said.letters)
          : engine::Duration{};
  return writing + SPOKEN_LINGER_TIME;
}

}  // namespace

SpokenLine::SpokenLine(const SpokenLineParts& parts)
    : sounds_(&parts.sounds.get()),
      voice_(parts.voice),
      width_(parts.width),
      tail_(parts.tail),
      motion_(parts.motion) {}

void SpokenLine::say(std::string_view text) {
  next_.reset();
  said_ = saidLineOf(text, width_ - (SIDES_PER_AXIS * SPOKEN_BUBBLE_PADDING),
                     motion_);
  voiceWordsShown(*said_);
}

void SpokenLine::sayNext(std::string_view text) {
  if (said_.has_value()) {
    next_ = std::string(text);
  } else {
    say(text);
  }
}

void SpokenLine::advance(engine::Duration elapsed) {
  if (!said_.has_value()) {
    return;
  }
  SaidLine& said = *said_;
  said.saidFor += elapsed;
  voiceWordsShown(said);
  if (said.saidFor < shownTimeOf(said, motion_)) {
    return;
  }
  said_.reset();
  if (std::optional<std::string> next = std::exchange(next_, std::nullopt)) {
    say(*next);
  }
}

bool SpokenLine::isSpeaking() const { return said_.has_value(); }

void SpokenLine::paint(IInterfaceCanvas& canvas,
                       sf::Vector2i bottomLeft) const {
  if (!said_.has_value()) {
    return;
  }
  const int height = bubbleHeightOf(said_->lines.size());
  const sf::IntRect bubble({bottomLeft.x, bottomLeft.y - height},
                           {width_, height});
  CutCornerBox(bubble,
               {.border = PANEL_BORDER_COLOR, .inside = PANEL_INSIDE_COLOR})
      .paint(canvas);
  paintTail(canvas, {bottomLeft.x + tail_, bottomLeft.y});
  paintSpeech(canvas, said_->lines,
              {.topLeft = bubble.position + sf::Vector2i(SPOKEN_BUBBLE_PADDING,
                                                         SPOKEN_BUBBLE_PADDING),
               .letters = lettersShownOf(*said_, motion_)});
}

void SpokenLine::voiceWordsShown(SaidLine& said) {
  const std::size_t letters = lettersShownOf(said, motion_);
  while (said.voicedWords < said.wordStarts.size() &&
         said.wordStarts.at(said.voicedWords) < letters) {
    if (!voice_.empty()) {
      sounds_->play(voice_[soundsPlayed_ % voice_.size()]);
      ++soundsPlayed_;
    }
    ++said.voicedWords;
  }
}

}  // namespace rtype::client
