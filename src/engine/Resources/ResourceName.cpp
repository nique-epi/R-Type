#include "ResourceName.hpp"
#include <algorithm>
#include <string>
#include "EngineException.hpp"
#include "ResourceConstants.hpp"

namespace rtype::engine {

namespace {

bool isResourceNameCharacter(char character) {
  const bool lowercase = character >= 'a' && character <= 'z';
  const bool digit = character >= '0' && character <= '9';
  return lowercase || digit ||
         std::ranges::find(RESOURCE_NAME_PUNCTUATION, character) !=
             RESOURCE_NAME_PUNCTUATION.end();
}

}  // namespace

void requireResourceName(const std::string& name) {
  const bool fits =
      !name.empty() && name.size() <= MAXIMUM_RESOURCE_NAME_LENGTH;
  if (!fits || !std::ranges::all_of(name, isResourceNameCharacter)) {
    throw InvalidResourceNameException(name, MAXIMUM_RESOURCE_NAME_LENGTH);
  }
}

}  // namespace rtype::engine
