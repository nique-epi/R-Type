#include "WrappedLines.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

std::vector<std::string> wrappedLinesOf(std::string_view text, int width,
                                        const IInterfaceCanvas& canvas) {
  std::vector<std::string> lines;
  std::size_t start = 0;
  while (start < text.size()) {
    const std::size_t end = text.find(WORD_SEPARATOR, start);
    const std::string_view word = text.substr(
        start,
        end == std::string_view::npos ? std::string_view::npos : end - start);
    std::string joined = lines.empty() ? std::string() : lines.back();
    joined += (joined.empty() ? "" : std::string(1, WORD_SEPARATOR));
    joined += word;
    if (!lines.empty() &&
        canvas.textWidth(joined, TextFont::Regular) <= width) {
      lines.back() = std::move(joined);
    } else {
      lines.emplace_back(word);
    }
    start = end == std::string_view::npos ? text.size() : end + 1;
  }
  return lines;
}

}  // namespace rtype::client
