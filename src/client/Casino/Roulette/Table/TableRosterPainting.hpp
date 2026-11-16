#pragma once

#include <string>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "TableRosterRow.hpp"

namespace rtype::client {

/** @brief Paints the panel of the room, its tag @p roomName, with a row of
 * @p rows per player seated: its color in a square, its name, and its value
 * on the right. Nothing without a room. */
void paintTableRoster(IInterfaceCanvas& canvas,
                      const std::vector<TableRosterRow>& rows,
                      const std::string& roomName);

}  // namespace rtype::client
