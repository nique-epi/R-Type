#pragma once

#include <functional>
#include <string_view>

namespace sf {
class Texture;
}  // namespace sf

namespace rtype::client {

/** @brief Finds the texture an asset id names; nullptr when none is
 * loaded. */
using TextureLookup = std::function<const sf::Texture*(std::string_view)>;

}  // namespace rtype::client
