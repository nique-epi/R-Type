#include <gtest/gtest.h>
#include "EventBus.hpp"
#include "TestService.hpp"
#include "TypeName.hpp"
#include "TypeNameTestConstants.hpp"

using rtype::engine::EventBus;
using rtype::engine::typeName;

/**
 * Given the TestService struct, declared outside any namespace
 * When its name is asked
 * Then it is "TestService"
 */
TEST(TypeName, NamesAStructOutsideANamespaceByItsName) {
  EXPECT_EQ(typeName(typeid(TestService)), TEST_SERVICE_NAME);
}

/**
 * Given the EventBus class, declared in rtype::engine
 * When its name is asked
 * Then it is "rtype::engine::EventBus"
 */
TEST(TypeName, NamesAClassWithItsNamespaces) {
  EXPECT_EQ(typeName(typeid(EventBus)), EVENT_BUS_NAME);
}
