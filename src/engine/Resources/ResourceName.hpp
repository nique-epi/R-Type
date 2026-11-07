#pragma once

#include <string>

namespace rtype::engine {

/**
 * @brief Checks that @p name can name a resource, and so its folder: 1 to
 * MAXIMUM_RESOURCE_NAME_LENGTH lowercase letters, digits, '-' and '_'.
 * @throws InvalidResourceNameException when it cannot.
 */
void requireResourceName(const std::string& name);

}  // namespace rtype::engine
