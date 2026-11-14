#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <span>
#include <string_view>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "SpeechKeyName.hpp"
#include "SpeechWord.hpp"

namespace rtype::client {

/** @brief The lines of a sentence said in a bubble, each a list of words. */
using SpeechLines = std::vector<std::vector<SpeechWord>>;

/** @brief Where a sentence is written, its first line at topLeft, and how
 * many of its letters show, a keycap counting as one letter. */
struct SpeechWriting {
  sf::Vector2i topLeft;
  std::size_t letters{};
};

/**
 * @returns The words of @p text in lines no wider than @p width: a word
 * between braces becomes a keycap, named by the entry of @p keyNames whose
 * token it is, or by itself; a word between stars is highlighted; a word
 * wider than a line has a line of its own.
 */
[[nodiscard]] SpeechLines speechLinesOf(std::string_view text,
                                        std::span<const SpeechKeyName> keyNames,
                                        int width);

/** @returns How many letters @p lines hold, as SpeechWriting counts them. */
[[nodiscard]] std::size_t speechLetterCountOf(const SpeechLines& lines);

/** @returns The letter each word of @p lines starts at, in order, as
 * SpeechWriting counts letters: the first word at 0. */
[[nodiscard]] std::vector<std::size_t> speechWordStartsOf(
    const SpeechLines& lines);

/** @brief Writes the first letters of @p lines, as many as @p writing says,
 * one line every SPEECH_LINE_HEIGHT pixels; a keycap shows once reached. */
void paintSpeech(IInterfaceCanvas& canvas, const SpeechLines& lines,
                 SpeechWriting writing);

}  // namespace rtype::client
