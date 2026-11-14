#include "SpokenLine.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <string>
#include <string_view>
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
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

}  // namespace

SpokenLine::SpokenLine(const SpokenLineParts& parts)
    : sounds_(&parts.sounds.get()),
      voice_(parts.voice),
      width_(parts.width),
      tail_(parts.tail),
      motion_(parts.motion) {}

void SpokenLine::say(std::string_view text) {
  start(text);
  voiceWordsUpTo(motion_ == MotionPreference::Full ? lettersShown() : 1);
  if (motion_ == MotionPreference::Reduced) {
    voicedWords_ = wordStarts_.size();
  }
}

void SpokenLine::sayNext(std::string_view text) {
  if (lines_.has_value()) {
    next_ = std::string(text);
  } else {
    say(text);
  }
}

void SpokenLine::advance(engine::Duration elapsed) {
  if (!lines_.has_value()) {
    return;
  }
  saidFor_ += elapsed;
  voiceWordsUpTo(lettersShown());
  if (saidFor_ < shownTime()) {
    return;
  }
  lines_.reset();
  if (next_.has_value()) {
    const std::string next = *next_;
    say(next);
  }
}

bool SpokenLine::isSpeaking() const { return lines_.has_value(); }

void SpokenLine::paint(IInterfaceCanvas& canvas,
                       sf::Vector2i bottomLeft) const {
  if (!lines_.has_value()) {
    return;
  }
  const int height = bubbleHeightOf(lines_->size());
  const sf::IntRect bubble({bottomLeft.x, bottomLeft.y - height},
                           {width_, height});
  CutCornerBox(bubble,
               {.border = PANEL_BORDER_COLOR, .inside = PANEL_INSIDE_COLOR})
      .paint(canvas);
  if (tail_.has_value()) {
    paintTail(canvas, {bottomLeft.x + *tail_, bottomLeft.y});
  }
  paintSpeech(canvas, *lines_,
              {.topLeft = bubble.position + sf::Vector2i(SPOKEN_BUBBLE_PADDING,
                                                         SPOKEN_BUBBLE_PADDING),
               .letters = lettersShown()});
}

void SpokenLine::start(std::string_view text) {
  lines_ = speechLinesOf(text, {},
                         width_ - (SIDES_PER_AXIS * SPOKEN_BUBBLE_PADDING));
  wordStarts_ = speechWordStartsOf(*lines_);
  next_.reset();
  voicedWords_ = 0;
  saidFor_ = {};
}

void SpokenLine::voiceWordsUpTo(std::size_t letters) {
  while (voicedWords_ < wordStarts_.size() &&
         wordStarts_.at(voicedWords_) < letters) {
    if (!voice_.empty()) {
      sounds_->play(voice_[soundsPlayed_ % voice_.size()]);
      ++soundsPlayed_;
    }
    ++voicedWords_;
  }
}

std::size_t SpokenLine::lettersShown() const {
  const std::size_t letters = speechLetterCountOf(*lines_);
  if (motion_ == MotionPreference::Reduced) {
    return letters;
  }
  return lettersTypedAt(saidFor_, SPOKEN_LETTER_TIME, letters);
}

engine::Duration SpokenLine::shownTime() const {
  const engine::Duration writing =
      motion_ == MotionPreference::Full
          ? typingTimeOf(SPOKEN_LETTER_TIME, speechLetterCountOf(*lines_))
          : engine::Duration{};
  return writing + SPOKEN_LINGER_TIME;
}

}  // namespace rtype::client
