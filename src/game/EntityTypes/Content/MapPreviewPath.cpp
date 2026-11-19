#include "MapPreviewPath.hpp"
#include <string>
#include <string_view>
#include "MapPreviewConstants.hpp"

namespace rtype::game {

std::string mapPreviewPathOf(std::string_view name) {
  std::string path{identifiers::MAP_PREVIEW_FOLDER};
  path += identifiers::MAP_PREVIEW_FOLDER_SEPARATOR;
  path += name;
  path += identifiers::MAP_PREVIEW_EXTENSION;
  return path;
}

}  // namespace rtype::game
