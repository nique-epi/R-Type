#pragma once

#include <string_view>

namespace rtype::identifiers {

/**
 * @brief Where the preview of a map lies in a resource and in a content pack:
 * the file MAP_PREVIEW_FOLDER/<name>MAP_PREVIEW_EXTENSION, its name a content
 * name (isContentName()), so its path is one a pack carries. The server names
 * it in the waiting room and lists it in its pack; the client draws it.
 */
constexpr std::string_view MAP_PREVIEW_FOLDER = "previews";
constexpr char MAP_PREVIEW_FOLDER_SEPARATOR = '/';
constexpr std::string_view MAP_PREVIEW_EXTENSION = ".png";

}  // namespace rtype::identifiers
