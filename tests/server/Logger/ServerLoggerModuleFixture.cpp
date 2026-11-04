#include "ServerLoggerModuleFixture.hpp"
#include <memory>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "IModule.hpp"
#include "ServerLoggerModule.hpp"

rtype::engine::Application ServerLoggerModuleFixture::applicationWithTheLogger()
    const {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(
      std::make_unique<rtype::server::ServerLoggerModule>(journalOptions()));
  return rtype::engine::Application(std::move(modules));
}
