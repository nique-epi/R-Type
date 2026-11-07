#include <filesystem>
#include <optional>
#include "JournalFolder.hpp"

namespace rtype::client {

std::optional<std::filesystem::path> journalFolder() { return std::nullopt; }

}  // namespace rtype::client
