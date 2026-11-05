#pragma once

#include <functional>
#include <string_view>

namespace rtype::client {

/**
 * @brief Tells whether this client can draw the entities of the type named
 * @p name: the game asks whether it has the appearance of that sheet, a
 * load-test bot, which draws nothing, says yes to every name.
 */
using DrawableTypeCheck = std::function<bool(std::string_view name)>;

}  // namespace rtype::client
