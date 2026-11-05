#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>

namespace rtype::engine {

/**
 * @brief A table of a data file: the table, the file it was read from, and its
 * dotted name (`difficulty.normal`), empty for the root of the file.
 *
 * It refers to the table and to the name of the file, so both must outlive
 * it. Every table belongs to the root parseDataFile() returned: keep that root
 * in a variable before taking a DataFileTable of it (see rootOf()).
 */
struct DataFileTable {
  std::reference_wrapper<const toml::table> table;
  std::string_view source;
  std::string name;
};

}  // namespace rtype::engine
