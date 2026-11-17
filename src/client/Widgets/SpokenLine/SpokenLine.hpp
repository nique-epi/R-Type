#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "SaidLine.hpp"
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
  /** @brief Plays a sound for each word of @p said whose first letter now
   * shows. */
  void voiceWordsShown(SaidLine& said);

  ISoundPlayer* sounds_;
  std::span<const char* const> voice_;
  int width_;
  int tail_;
  MotionPreference motion_;
  /** @brief The line said now; empty while silent. */
  std::optional<SaidLine> said_;
  std::optional<std::string> next_;
  /** @brief How many sounds the voice played, which picks the next one. */
  std::size_t soundsPlayed_{};
};

}  // namespace rtype::client
