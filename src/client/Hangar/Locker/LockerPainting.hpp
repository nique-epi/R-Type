#pragma once

#include "IInterfaceCanvas.hpp"
#include "LockerView.hpp"

namespace rtype::client {

/** @brief Paints the locker of the hangar at LOCKER_TOP_LEFT: brushed steel,
 * a name plate, a cell per item with its icon, a gold tag on the items the
 * ship wears, a cyan frame round the chosen one, and a glass door. */
void paintLocker(IInterfaceCanvas& canvas, LockerView view);

}  // namespace rtype::client
