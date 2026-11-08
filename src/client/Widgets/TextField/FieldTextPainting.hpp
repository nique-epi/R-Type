#pragma once

#include "FieldText.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Writes the text of @p field FIELD_TEXT_PADDING inside its box, in
 * the regular font, then the caret after its last letter when it asks for
 * one. */
void paintFieldText(IInterfaceCanvas& canvas, const FieldText& field);

}  // namespace rtype::client
