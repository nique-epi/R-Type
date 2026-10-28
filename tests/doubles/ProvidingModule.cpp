#include "ProvidingModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "EventBus.hpp"
#include "InstallContext.hpp"
#include "TestService.hpp"

ProvidingModule::ProvidingModule(std::string name, TestService& service,
                                 rtype::engine::EventBus& bus)
    : name_(std::move(name)), service_(&service), bus_(&bus) {}

std::string ProvidingModule::name() const { return name_; }

std::vector<std::string> ProvidingModule::dependencies() const { return {}; }

void ProvidingModule::install(rtype::engine::InstallContext& context) {
  context.provide(*service_);
  context.provide(*bus_);
}

void ProvidingModule::start() {}

void ProvidingModule::stop() {}
