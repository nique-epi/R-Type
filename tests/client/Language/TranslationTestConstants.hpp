#pragma once

#include <string_view>
#include "Translation.hpp"

constexpr std::string_view SOME_ENGLISH = "MUSIC";
constexpr std::string_view SOME_FRENCH = "MUSIQUE";
constexpr rtype::client::Translation SOME_TRANSLATION{.english = SOME_ENGLISH,
                                                      .french = SOME_FRENCH};
