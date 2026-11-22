#pragma once

#include <cstddef>

namespace rtype::client {

/** @brief The row selected in a tab of the options, kept while the tabs are
 * built again in another language. */
struct SelectedRow {
  std::size_t tab{};
  std::size_t row{};
};

}  // namespace rtype::client
