#pragma once

#include <string_view>
#include "TextMark.hpp"

constexpr std::string_view COUNT_MARK = "{COUNT}";
constexpr rtype::client::TextMark THREE_AS_COUNT{.mark = COUNT_MARK,
                                                 .value = "3"};
constexpr std::string_view MARK_IN_THE_MIDDLE = "TIRAGE DANS {COUNT} S";
constexpr std::string_view MIDDLE_FILLED = "TIRAGE DANS 3 S";
constexpr std::string_view MARK_TWICE = "{COUNT} ET {COUNT}";
constexpr std::string_view FIRST_OF_TWO_FILLED = "3 ET {COUNT}";
constexpr std::string_view NO_MARK = "PRÊT";
