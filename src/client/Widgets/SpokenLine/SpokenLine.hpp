#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "SpeechLines.hpp"
#include "SpokenLineParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief A line a character says in a bubble: written a letter every
 * SPOKEN_LETTER_TIME, a sound of its voice as each word starts, shown
 * SPOKEN_LINGER_TIME once whole, then gone, or followed by the line said
 * next.
 *
 * A line is written as speechLinesOf() cuts it, to the width of the bubble
 * less its padding; it names no key, so a word between braces shows itself.
 * While the player asks for less motion a line shows whole at once, with one
 * sound. A new say() replaces the line shown and forgets the line next.
 */
class SpokenLine {
 public:
  explicit SpokenLine(const SpokenLineParts& parts);

  /** @brief Starts saying @p text now, in place of anything said, and plays
   * the sound of its first word. */
  void say(std::string_view text);
  /** @brief Says @p text once the line shown is gone, at once when none
   * shows. */
  void sayNext(std::string_view text);
  /** @brief Writes the letters due, plays the sound of each word they start,
   * and ends the line once it lingered, starting the line next if any. */
  void advance(engine::Duration elapsed);
  /** @returns Whether a bubble shows. */
  [[nodiscard]] bool isSpeaking() const;
  /** @brief Paints the bubble, its bottom-left corner at @p bottomLeft, its
   * tail under it, and the letters written; nothing while silent. */
  void paint(IInterfaceCanvas& canvas, sf::Vector2i bottomLeft) const;

 private:
  /** @brief Takes @p text as the line said from now on, nothing said of it
   * yet, and forgets the line next. */
  void start(std::string_view text);
  /** @brief Plays a sound for each word whose first letter is among the
   * first @p letters. */
  void voiceWordsUpTo(std::size_t letters);
  /** @returns How many letters of the line show now. */
  [[nodiscard]] std::size_t lettersShown() const;
  /** @returns How long the line shows in all: its writing, then its
   * lingering. */
  [[nodiscard]] engine::Duration shownTime() const;

  ISoundPlayer* sounds_;
  std::span<const char* const> voice_;
  int width_;
  std::optional<int> tail_;
  MotionPreference motion_;
  std::optional<SpeechLines> lines_;
  std::optional<std::string> next_;
  /** @brief The letter each word of the line starts at, in order. */
  std::vector<std::size_t> wordStarts_;
  /** @brief How many words of the line were given their sound. */
  std::size_t voicedWords_{};
  /** @brief How many sounds the voice played, which picks the next one. */
  std::size_t soundsPlayed_{};
  /** @brief How long the line has been said. */
  engine::Duration saidFor_{};
};

}  // namespace rtype::client
