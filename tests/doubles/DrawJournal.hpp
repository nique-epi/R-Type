#pragma once

#include <string>
#include <vector>

/** @brief The order in which recording doubles were laid on a target, shared
 * by the doubles of one test: each appends its name when it is shown. */
struct DrawJournal {
  std::vector<std::string> laid;
};

/** @brief Names the doubles write in a DrawJournal. */
constexpr const char* ENTITY_LAYER_LAID = "entity layer";
constexpr const char* INTERFACE_CANVAS_LAID = "interface canvas";
