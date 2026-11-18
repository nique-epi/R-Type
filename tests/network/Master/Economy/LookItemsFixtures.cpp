#include "LookItemsFixtures.hpp"
#include <string>
#include <vector>

namespace rtype::network::testing {

std::vector<std::string> looksInNoOrder() {
  return {"mustache", "no_such_look", "crown", "top_hat"};
}

std::vector<std::string> looksInCatalogueOrder() {
  return {"top_hat", "crown", "mustache", "no_such_look"};
}

}  // namespace rtype::network::testing
