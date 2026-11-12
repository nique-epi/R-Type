#pragma once

#include <string>
#include "DataFilePlace.hpp"

namespace rtype::engine {

/** @brief One text of an array of a data file, with where the file writes it,
 * for an error about that text. */
struct DataFileText {
  std::string text;
  DataFilePlace place;
};

}  // namespace rtype::engine
