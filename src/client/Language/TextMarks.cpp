#include "TextMarks.hpp"
#include <cstddef>
#include <string>
#include "TextMark.hpp"

namespace rtype::client {

std::string withMark(std::string text, TextMark mark) {
  const std::size_t found = text.find(mark.mark);
  if (found != std::string::npos) {
    text.replace(found, mark.mark.size(), mark.value);
  }
  return text;
}

}  // namespace rtype::client
