#pragma once

#include <vector>
#include "EntityMessage.hpp"
#include "GameEventHistory.hpp"
#include "IEntityMessageSource.hpp"
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief The entity messages of the game: from the start of the game, it keeps
 * the EntitySpawned, WorldState and EntityDestroyed of the server, and the
 * first copy of each GameEvent (see GameEventHistory), in the order they
 * arrived, until drainEntityMessages() hands them over; leaving the session
 * drops those not handed over and forgets the events seen. The game screen
 * drains them every frame, behind the options too, so they never pile up for
 * longer than the loading of the game.
 *
 * The game starts with a GameStarted, or with a WorldState when the
 * GameStarted was lost: the WorldState tells the waiting room. The first
 * GameEnded of a game goes to the game screen with the messages, and the
 * session is no longer in a game; the copies the server repeats are dropped.
 * Before the game started nothing is kept, since the first WorldState of the
 * game lists every entity anyway.
 */
class EntityMessageQueue final : public IEntityMessageSource,
                                 public ISessionObserver {
 public:
  /**
   * @param dispatcher Where the handlers of the entity messages are
   * registered; it must outlive the queue.
   * @param room Tells whether the game started, and is told that it did and
   * that it ended; it must outlive the queue.
   */
  EntityMessageQueue(network::MessageDispatcher& dispatcher, WaitingRoom& room);

  /** @brief Counts @p elapsed against the memory of the game events. */
  void advance(engine::Duration elapsed);

  void drainEntityMessages(std::vector<EntityMessage>& destination) override;

  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;

  /** @brief Drops the messages not handed over and forgets the events
   * seen. */
  void sessionClosed() override;

  void sessionHeld(engine::Duration elapsed) override;

 private:
  /** @brief Keeps @p message until the next drain, once the game started. */
  void keep(EntityMessage message);

  WaitingRoom* room_;
  std::vector<EntityMessage> messages_;
  GameEventHistory gameEvents_;
};

}  // namespace rtype::client
