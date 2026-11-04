#pragma once

#include <string>
#include <string_view>

namespace rtype::testing {

/** @brief "папка" (Cyrillic for "folder"), a folder name outside the code page
 * of a Windows set up for Western Europe, as a UTF-8 string. */
inline const std::u8string NON_ANSI_FOLDER_NAME = u8"папка";

/** @brief The bytes of NON_ANSI_FOLDER_NAME, written out so they do not depend
 * on the encoding of this source file. */
constexpr std::string_view NON_ANSI_FOLDER_BYTES =
    "\xd0\xbf\xd0\xb0\xd0\xbf\xd0\xba\xd0\xb0";

}  // namespace rtype::testing
