#pragma once

#include "SheetVisualNames.hpp"

namespace rtype::game {

/**
 * @brief Component of every body made from a sheet: the effects and the looks
 * its sheet names, so the systems that carry out what a script asks for check
 * a name against the sheet of the body. It views the SheetVisualNames of the
 * EntitySheet, which outlives every body made from it.
 */
struct VisualNames {
  const SheetVisualNames* names{nullptr};
};

}  // namespace rtype::game
