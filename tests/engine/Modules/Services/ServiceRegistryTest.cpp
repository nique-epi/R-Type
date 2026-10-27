#include <gtest/gtest.h>
#include "EventBus.hpp"
#include "ServiceRegistry.hpp"
#include "TestService.hpp"

using rtype::engine::EventBus;
using rtype::engine::ServiceRegistry;

/**
 * Given a registry given a TestService
 * When a TestService is looked for
 * Then the one given is returned
 */
TEST(ServiceRegistry, FindReturnsTheProvidedService) {
  ServiceRegistry services;
  TestService service;
  services.provide(service);

  EXPECT_EQ(services.find<TestService>(), &service);
}

/**
 * Given a registry given no TestService
 * When a TestService is looked for
 * Then nothing is returned
 */
TEST(ServiceRegistry, FindOfATypeNeverProvidedReturnsNull) {
  const ServiceRegistry services;

  EXPECT_EQ(services.find<TestService>(), nullptr);
}

/**
 * Given a registry given a TestService
 * When a second TestService is given
 * Then the registry refuses it
 */
TEST(ServiceRegistry, SecondProvideOfATypeIsRefused) {
  ServiceRegistry services;
  TestService first;
  TestService second;
  services.provide(first);

  EXPECT_FALSE(services.provide(second));
}

/**
 * Given a registry given a TestService, then a second one
 * When a TestService is looked for
 * Then the first one is returned
 */
TEST(ServiceRegistry, SecondProvideOfATypeKeepsTheFirst) {
  ServiceRegistry services;
  TestService first;
  TestService second;
  services.provide(first);
  services.provide(second);

  EXPECT_EQ(services.find<TestService>(), &first);
}

/**
 * Given a registry given a TestService and an EventBus
 * When an EventBus is looked for
 * Then the EventBus given is returned
 */
TEST(ServiceRegistry, KeepsServicesOfDifferentTypesApart) {
  ServiceRegistry services;
  TestService service;
  EventBus bus;
  services.provide(service);
  services.provide(bus);

  EXPECT_EQ(services.find<EventBus>(), &bus);
}
