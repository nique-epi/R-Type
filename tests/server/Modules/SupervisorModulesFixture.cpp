#include "SupervisorModulesFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReader.hpp"
#include "ConfigurationReport.hpp"
#include "ConfigurationSource.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "ExecutablePath.hpp"
#include "GameLineRule.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "LoopStages.hpp"
#include "MessagePayload.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerInformation.hpp"
#include "SupervisorModules.hpp"
#include "SupervisorModulesTestConstants.hpp"
#include "TimeConstants.hpp"
#include "UdpTestClient.hpp"

SupervisorModulesFixture::SupervisorModulesFixture()
    : port_(freeUdpPort()),
      portText_(std::to_string(port_)),
      journalText_(journal().path().string()) {}

SupervisorModulesFixture::~SupervisorModulesFixture() {
  std::error_code ignored;
  std::filesystem::remove(gameConfigurationFile(), ignored);
}

void SupervisorModulesFixture::writeConfiguration(
    const std::vector<std::string>& lines) const {
  std::string text;
  for (const std::string& line : lines) {
    text += line + '\n';
  }
  folder_.write(rtype::server::SERVER_CONFIGURATION_FILE_NAME, text);
}

std::unique_ptr<rtype::engine::Application>
SupervisorModulesFixture::supervisor(
    const std::vector<std::string_view>& options) {
  std::vector<std::string_view> arguments{
      rtype::engine::PORT_OPTION, portText_, rtype::logging::FILE_OPTION,
      journalText_, rtype::logging::NO_STDERR_OPTION};
  arguments.insert(arguments.end(), options.begin(), options.end());
  const rtype::server::LaunchOptions launchOptions =
      rtype::server::LaunchOptions::read(arguments);
  auto modules = rtype::server::supervisorModules(launchOptions, stopRequest_,
                                                  report_.handedOverLines);
  auto step = rtype::engine::configurationStep(
      rtype::engine::ConfigurationSource{
          .file =
              folder_.path() / rtype::server::SERVER_CONFIGURATION_FILE_NAME,
          .forcedLines = launchOptions.forcedLines(),
          .handOver = rtype::server::gameLineRule(modules)},
      report_);
  return std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      std::move(modules), std::move(step));
}

const rtype::engine::ConfigurationReport& SupervisorModulesFixture::report()
    const {
  return report_;
}

std::string SupervisorModulesFixture::gameConfigurationText() const {
  std::ifstream stream{gameConfigurationFile()};
  return std::string{std::istreambuf_iterator<char>{stream},
                     std::istreambuf_iterator<char>{}};
}

std::filesystem::path SupervisorModulesFixture::gameConfigurationFile() const {
  return rtype::engine::executableFolder() /
         (std::string{GAMES_FILE_PREFIX} + portText_ +
          std::string{GAMES_FILE_SUFFIX});
}

rtype::network::ServerInformation SupervisorModulesFixture::askInformation(
    const rtype::engine::Application& application) {
  client_.send(
      rtype::network::Endpoint{
          .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
          .port = port_},
      informationRequestDatagram());
  for (std::size_t tick = 0; tick < INFORMATION_WAIT_TICKS; ++tick) {
    application.runStages(rtype::engine::SIMULATION_TICK_DURATION);
    const std::vector<ReceivedAt> received =
        client_.waitForDatagrams(1, ONE_TICK_WAIT);
    if (!received.empty()) {
      return rtype::network::decodeMessage<rtype::network::ServerInformation>(
          rtype::network::DatagramCodec::splitMessages(received.front().payload)
              .at(0)
              .payload);
    }
  }
  ADD_FAILURE() << "the supervisor did not answer the information request";
  return rtype::network::ServerInformation{};
}
