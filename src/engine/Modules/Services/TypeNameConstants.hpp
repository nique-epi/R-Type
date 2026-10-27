#pragma once

#include <array>
#include <string_view>

namespace rtype::engine {

/**
 * @brief The words MSVC writes before the name of a type in
 * std::type_info::name(), as in "struct TestService", which typeName() removes.
 */
constexpr std::array<std::string_view, 4> TYPE_KEYWORD_PREFIXES{
    "class ", "struct ", "union ", "enum "};

}  // namespace rtype::engine
