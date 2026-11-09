#pragma once

#include <string>
#include <string_view>

namespace rtype::client::testing {

/** @returns A manifest of 1.1.0 with one note, whose Linux package is the
 * JSON object @p package. */
[[nodiscard]] std::string manifestWithPackage(std::string_view package);

/** @returns A manifest of 1.1.0 with a good Linux package, whose notes are
 * the JSON array @p notes. */
[[nodiscard]] std::string manifestWithNotes(std::string_view notes);

}  // namespace rtype::client::testing
