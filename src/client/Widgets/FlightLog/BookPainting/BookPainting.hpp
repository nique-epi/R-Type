#pragma once

#include "BookPose.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Paints the book in its pose, from the flight log sheet: closed with
 * its side and its shadow, open flat, or its cover turning column by
 * column. */
void paintBook(IInterfaceCanvas& canvas, BookPose pose);

}  // namespace rtype::client
