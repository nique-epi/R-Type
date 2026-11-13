#include "WrappedLines.hpp"
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "LineWrapping.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Where the letter after the one starting at @p start starts in
 * @p word: past the bytes that continue a letter of UTF-8. */
std::size_t nextLetterOf(std::string_view word, std::size_t start) {
  std::size_t next = start + 1;
  while (next < word.size() && (static_cast<unsigned char>(word[next]) &
                                UTF8_LEAD_MASK) == UTF8_CONTINUATION_BITS) {
    ++next;
  }
  return next;
}

/** @brief How a line is measured: the width and font it is cut to, and the
 * canvas that measures it, which outlives the cut.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct LineMeasure {
  LineWrapping wrapping;
  std::reference_wrapper<const IInterfaceCanvas> canvas;
};

/** @returns Whether @p text fits on one line of @p measure. */
bool fitsOn(const LineMeasure& measure, std::string_view text) {
  return measure.canvas.get().textWidth(text, measure.wrapping.font) <=
         measure.wrapping.width;
}

/** @brief Adds @p word to @p lines cut between two letters: each piece holds
 * as many letters as fit on a line, at least one. */
void addBrokenWord(std::vector<std::string>& lines, std::string_view word,
                   const LineMeasure& measure) {
  std::string piece;
  for (std::size_t start = 0; start < word.size();) {
    const std::size_t next = nextLetterOf(word, start);
    const std::string_view letter = word.substr(start, next - start);
    std::string longer = piece + std::string(letter);
    if (!piece.empty() && !fitsOn(measure, longer)) {
      lines.push_back(std::move(piece));
      piece = std::string(letter);
    } else {
      piece = std::move(longer);
    }
    start = next;
  }
  lines.push_back(std::move(piece));
}

/** @brief Adds @p word to @p lines: after the last line when both fit on
 * one, on a line of its own when it fits alone, cut otherwise. */
void addWord(std::vector<std::string>& lines, std::string_view word,
             const LineMeasure& measure) {
  if (!lines.empty()) {
    std::string joined = lines.back();
    joined += WORD_SEPARATOR;
    joined += word;
    if (fitsOn(measure, joined)) {
      lines.back() = std::move(joined);
      return;
    }
  }
  if (fitsOn(measure, word)) {
    lines.emplace_back(word);
    return;
  }
  addBrokenWord(lines, word, measure);
}

}  // namespace

std::vector<std::string> wrappedLinesOf(std::string_view text,
                                        LineWrapping wrapping,
                                        const IInterfaceCanvas& canvas) {
  const LineMeasure measure{.wrapping = wrapping, .canvas = canvas};
  std::vector<std::string> lines;
  std::size_t start = 0;
  while (start < text.size()) {
    const std::size_t end = text.find(WORD_SEPARATOR, start);
    const std::string_view word = text.substr(
        start,
        end == std::string_view::npos ? std::string_view::npos : end - start);
    if (!word.empty()) {
      addWord(lines, word, measure);
    }
    start = end == std::string_view::npos ? text.size() : end + 1;
  }
  return lines;
}

}  // namespace rtype::client
