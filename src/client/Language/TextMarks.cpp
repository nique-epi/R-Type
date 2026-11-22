#include "TextMarks.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include "TextMark.hpp"

namespace rtype::client {

std::string withMark(std::string_view text, TextMark mark) {
  std::string marked(text);
  const std::size_t found = marked.find(mark.mark);
  if (found != std::string::npos) {
    marked.replace(found, mark.mark.size(), mark.value);
  }
  return marked;
}

}  // namespace rtype::client
