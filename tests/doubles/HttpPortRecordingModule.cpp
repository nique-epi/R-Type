#include "HttpPortRecordingModule.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "HttpPortRecordingModuleConstants.hpp"
#include "InstallContext.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"

HttpPortRecordingModule::HttpPortRecordingModule(std::uint16_t& port)
    : port_(&port) {}

std::string HttpPortRecordingModule::name() const {
  return std::string{HTTP_PORT_RECORDING_MODULE_NAME};
}

std::vector<std::string> HttpPortRecordingModule::dependencies() const {
  return {std::string{rtype::master::HTTP_MODULE_NAME}};
}

void HttpPortRecordingModule::install(rtype::engine::InstallContext& context) {
  server_ = &context.require<rtype::master::MasterHttpServer>();
}

void HttpPortRecordingModule::start() { *port_ = server_->port(); }

void HttpPortRecordingModule::stop() {}
