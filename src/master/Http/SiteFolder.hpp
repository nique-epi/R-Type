#pragma once

#include <filesystem>
#include <functional>

namespace rtype::master {

/**
 * @brief What names the folder of the web site, read once by
 * MasterHttpServer::open(), after the settings of the master are applied.
 */
using SiteFolder = std::function<std::filesystem::path()>;

}  // namespace rtype::master
