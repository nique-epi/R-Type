#pragma once

#include <array>
#include <cstddef>
#include "Language.hpp"

namespace rtype::client {

/** @brief Every language, in the order the options list them. */
constexpr std::array LANGUAGES{Language::English, Language::French};
constexpr std::size_t LANGUAGE_COUNT = LANGUAGES.size();

}  // namespace rtype::client
