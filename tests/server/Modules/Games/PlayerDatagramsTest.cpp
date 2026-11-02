#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "PlayerDatagrams.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::server::keepDatagramsOf;
using rtype::server::PlayerRoster;
using rtype::server::testing::playerEndpoint;

/**
 * Given a roster holding the first and the third player
 * When the datagrams of the first, second, third and first player again are
 * sorted for that roster
 * Then only those of the first and the third are kept, in the order they came
 */
TEST(PlayerDatagrams, KeepsOnlyTheDatagramsOfSeatedPlayersInTheirOrder) {
  PlayerRoster roster;
  roster.add(playerEndpoint(0), PLAYER_NICKNAME, PLAYER_TOKEN_BASE);
  roster.add(playerEndpoint(2), PLAYER_NICKNAME, PLAYER_TOKEN_BASE + 2);
  const std::vector<IncomingDatagram> datagrams{
      IncomingDatagram{.sender = playerEndpoint(0), .payload = {}},
      IncomingDatagram{.sender = playerEndpoint(1), .payload = {}},
      IncomingDatagram{.sender = playerEndpoint(2), .payload = {}},
      IncomingDatagram{.sender = playerEndpoint(0), .payload = {}}};
  std::vector<IncomingDatagram> kept;

  keepDatagramsOf(roster, datagrams, kept);

  std::vector<Endpoint> senders;
  senders.reserve(kept.size());
  for (const IncomingDatagram& datagram : kept) {
    senders.push_back(datagram.sender);
  }
  EXPECT_EQ(senders,
            (std::vector<Endpoint>{playerEndpoint(0), playerEndpoint(2),
                                   playerEndpoint(0)}));
}

/**
 * Given datagrams kept for a roster on an earlier tick
 * When the datagrams of a tick none of whose senders is seated are sorted
 * Then nothing is kept: what the earlier tick kept is gone
 */
TEST(PlayerDatagrams, ForgetsWhatAnEarlierTickKept) {
  PlayerRoster roster;
  roster.add(playerEndpoint(0), PLAYER_NICKNAME, PLAYER_TOKEN_BASE);
  std::vector<IncomingDatagram> kept{
      IncomingDatagram{.sender = playerEndpoint(0), .payload = {}}};

  keepDatagramsOf(roster,
                  std::vector<IncomingDatagram>{IncomingDatagram{
                      .sender = playerEndpoint(1), .payload = {}}},
                  kept);

  EXPECT_TRUE(kept.empty());
}
