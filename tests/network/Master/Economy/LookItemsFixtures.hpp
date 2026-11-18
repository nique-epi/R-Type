#pragma once

#include <string>
#include <vector>

namespace rtype::network::testing {

/** @returns Looks in no order, a name of no look among them. */
[[nodiscard]] std::vector<std::string> looksInNoOrder();

/** @returns The same looks in the order of the catalogue: top hat, crown,
 * mustache, then the unknown. */
[[nodiscard]] std::vector<std::string> looksInCatalogueOrder();

}  // namespace rtype::network::testing
