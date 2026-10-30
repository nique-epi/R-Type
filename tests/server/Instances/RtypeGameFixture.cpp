#include "RtypeGameFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include "IGameReplication.hpp"
#include "MessageType.hpp"
#include "ReplicationModule.hpp"
#include "RtypeRulesModule.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

void RtypeGameFixture::installReplication() { install(replicationModule_); }

IGameReplication& RtypeGameFixture::replicationOfTheWorld() {
  installReplication();
  IGameReplication& replication = *provided<IGameReplication>();
  replication.onWorldBuilt(world());
  return replication;
}

void RtypeGameFixture::startBoth() {
  installReplication();
  install(rules_);
  replicationModule_.start();
  rules_.start();
}

void RtypeGameFixture::stopBoth() {
  rules_.stop();
  replicationModule_.stop();
}

ReplicationModule& RtypeGameFixture::replicationModule() {
  return replicationModule_;
}

RtypeRulesModule& RtypeGameFixture::rules() { return rules_; }

void RtypeGameFixture::expectFirstMessageSentTo(std::size_t index,
                                                network::MessageType type) {
  const auto received = sentTo(sentDatagrams(), playerEndpoint(index));
  ASSERT_FALSE(received.empty());
  EXPECT_EQ(onlyMessageTypeOf(received.front()), type);
}

}  // namespace rtype::server::testing
