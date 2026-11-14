#include "SpeechLines.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "KeyCap.hpp"
#include "SpeechKeyName.hpp"
#include "SpeechLinesConstants.hpp"
#include "SpeechWord.hpp"
#include "SpeechWordLook.hpp"
#include "Typing.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @brief The part of a word that shows: its text, how it is written, then
 * its punctuation. */
struct ShownWord {
  std::string_view text;
  SpeechWordLook look{};
  std::string_view suffix;
};

/** @returns Whether a word is wrapped in the two marks. */
bool isWrapped(std::string_view word, char opening, char closing) {
  return word.size() > 1 && word.front() == opening && word.back() == closing;
}

/** @returns A word wrapped in marks, without them. */
std::string_view insideOf(std::string_view wrapped) {
  return wrapped.substr(1, wrapped.size() - 2);
}

/** @returns The name a keycap shows: the name of the entry of @p keyNames
 * whose token is @p word, or the word itself. */
std::string keyCapNameOf(std::string_view word,
                         std::span<const SpeechKeyName> keyNames) {
  for (const SpeechKeyName& keyName : keyNames) {
    if (keyName.token == word) {
      return keyName.name;
    }
  }
  return std::string(word);
}

/** @returns A word of a sentence as the bubble writes it, the punctuation
 * that ends a keycap or a highlighted word apart. */
SpeechWord wordOf(std::string_view token,
                  std::span<const SpeechKeyName> keyNames) {
  const std::size_t coreLength = token.find_last_not_of(SPEECH_PUNCTUATION) + 1;
  const std::string_view core = token.substr(0, coreLength);
  std::string suffix(token.substr(coreLength));
  if (isWrapped(core, KEY_CAP_OPENING, KEY_CAP_CLOSING)) {
    return {.text = keyCapNameOf(insideOf(core), keyNames),
            .look = SpeechWordLook::KeyCap,
            .suffix = std::move(suffix)};
  }
  if (isWrapped(core, INTERFACE_WORD_MARK, INTERFACE_WORD_MARK)) {
    return {.text = std::string(insideOf(core)),
            .look = SpeechWordLook::Highlighted,
            .suffix = std::move(suffix)};
  }
  return {
      .text = std::string(token), .look = SpeechWordLook::Plain, .suffix = {}};
}

/** @returns The words of a sentence, split on WORD_SEPARATOR. */
std::vector<SpeechWord> wordsOf(std::string_view text,
                                std::span<const SpeechKeyName> keyNames) {
  std::vector<SpeechWord> words;
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
int widthOf(const ShownWord& word) {
  const int text =
      word.look == SpeechWordLook::KeyCap
          ? KeyCap(std::string(word.text), KeyCap::Edge::Plain).width()
          : regularTextWidth(word.text);
  return text + regularTextWidth(word.suffix);
}

ShownWord wholeOf(const SpeechWord& word) {
  return {.text = word.text, .look = word.look, .suffix = word.suffix};
}

/** @returns How many letters the text of a word holds, a keycap one. */
std::size_t textLettersOf(const SpeechWord& word) {
  return word.look == SpeechWordLook::KeyCap ? 1 : letterCountOf(word.text);
}

std::size_t lettersOf(const SpeechWord& word) {
  return textLettersOf(word) + letterCountOf(word.suffix);
}

/** @returns The part of a word its first @p letters letters show, at least
 * one; a keycap shows whole. */
ShownWord shownPartOf(const SpeechWord& word, std::size_t letters) {
  const std::size_t textLetters = textLettersOf(word);
  if (letters <= textLetters) {
    return {.text = word.look == SpeechWordLook::KeyCap
                        ? std::string_view(word.text)
                        : typedPrefix(word.text, letters),
            .look = word.look,
            .suffix = {}};
  }
  return {.text = word.text,
          .look = word.look,
          .suffix = typedPrefix(word.suffix, letters - textLetters)};
}

/** @brief Writes a word at its place on a line, then its punctuation.
 * @returns How wide it was written. */
int paintWord(IInterfaceCanvas& canvas, const ShownWord& word,
              sf::Vector2i topLeft) {
  int width = 0;
  if (word.look == SpeechWordLook::KeyCap) {
    const KeyCap keyCap(std::string(word.text), KeyCap::Edge::Plain);
    keyCap.paint(canvas, topLeft - sf::Vector2i(0, CONTROL_TEXT_TOP));
    width = keyCap.width();
  } else {
    canvas.write(word.text, topLeft,
                 word.look == SpeechWordLook::Highlighted
                     ? DIALOG_HEADING_STYLE
                     : DIALOG_MESSAGE_STYLE);
    width = regularTextWidth(word.text);
  }
  if (!word.suffix.empty()) {
    canvas.write(word.suffix, topLeft + sf::Vector2i(width, 0),
                 DIALOG_MESSAGE_STYLE);
  }
  return width + regularTextWidth(word.suffix);
}

}  // namespace

SpeechLines speechLinesOf(std::string_view text,
                          std::span<const SpeechKeyName> keyNames, int width) {
  SpeechLines lines;
  int used = 0;
  for (SpeechWord& word : wordsOf(text, keyNames)) {
    const int wordWidth = widthOf(wholeOf(word));
    if (lines.empty() || used + SPEECH_WORD_GAP + wordWidth > width) {
      lines.emplace_back();
      used = -SPEECH_WORD_GAP;
    }
    used += SPEECH_WORD_GAP + wordWidth;
    lines.back().push_back(std::move(word));
  }
  return lines;
}

std::size_t speechLetterCountOf(const SpeechLines& lines) {
  std::size_t letters = 0;
  for (const std::vector<SpeechWord>& line : lines) {
    for (const SpeechWord& word : line) {
      letters += lettersOf(word);
    }
  }
  return letters;
}

std::vector<std::size_t> speechWordStartsOf(const SpeechLines& lines) {
  std::vector<std::size_t> starts;
  std::size_t letters = 0;
  for (const std::vector<SpeechWord>& line : lines) {
    for (const SpeechWord& word : line) {
      starts.push_back(letters);
      letters += lettersOf(word);
    }
  }
  return starts;
}

void paintSpeech(IInterfaceCanvas& canvas, const SpeechLines& lines,
                 SpeechWriting writing) {
  std::size_t left = writing.letters;
  sf::Vector2i lineTopLeft = writing.topLeft;
  for (const std::vector<SpeechWord>& line : lines) {
    sf::Vector2i wordTopLeft = lineTopLeft;
    for (const SpeechWord& word : line) {
      if (left == 0) {
        return;
      }
      const ShownWord shown = shownPartOf(word, left);
      left -= std::min(left, lettersOf(word));
      wordTopLeft.x += paintWord(canvas, shown, wordTopLeft) + SPEECH_WORD_GAP;
    }
    lineTopLeft.y += SPEECH_LINE_HEIGHT;
  }
}

}  // namespace rtype::client
