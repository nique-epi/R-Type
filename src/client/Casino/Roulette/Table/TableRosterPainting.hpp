#pragma once

#include <vector>
#include "IInterfaceCanvas.hpp"
#include "TableRosterRow.hpp"

namespace rtype::client {

/** @brief Paints the panel TABLE with a row of @p rows per pilot seated: its
 * color in a square, its name, SPIN when it asked to spin, and its value on
 * the right. Nothing when nobody sits. */
void paintTableRoster(IInterfaceCanvas& canvas,
                      const std::vector<TableRosterRow>& rows);

}  // namespace rtype::client
