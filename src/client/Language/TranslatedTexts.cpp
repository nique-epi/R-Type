#include "TranslatedTexts.hpp"
#include <span>
#include <string>
#include <vector>
#include "Language.hpp"
#include "Translation.hpp"

namespace rtype::client {

std::vector<std::string> textsIn(std::span<const Translation> labels,
                                 Language language) {
  std::vector<std::string> texts;
  texts.reserve(labels.size());
  for (const Translation& label : labels) {
    texts.emplace_back(label.in(language));
  }
  return texts;
}

}  // namespace rtype::client
