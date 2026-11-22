#pragma once

#include <string_view>
#include "Language.hpp"

namespace rtype::client {

/**
 * @brief One text of the client, in every language the client speaks.
 *
 * Always built with designated initializers naming both members: GCC refuses a
 * Translation that leaves one out (-Wmissing-field-initializers is an error of
 * the CI), so no text ships without its French.
 */
struct Translation {
  std::string_view english;
  std::string_view french;

  /** @returns The text in @p language. */
  [[nodiscard]] constexpr std::string_view in(Language language) const;
};

constexpr std::string_view Translation::in(Language language) const {
  switch (language) {
    case Language::English:
      return english;
    case Language::French:
      return french;
  }
  return english;
}

}  // namespace rtype::client
