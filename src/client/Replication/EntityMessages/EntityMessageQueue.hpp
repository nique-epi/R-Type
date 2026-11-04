#pragma once

#include <vector>
#include "EnemyActionHistory.hpp"
#include "EntityMessage.hpp"
#include "GameEventHistory.hpp"
#include "GameLoadedReminder.hpp"
#include "IEntityMessageSource.hpp"
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"
#include "WorldStateAssembler.hpp"

namespace rtype::client {

/**
 * @brief The entity messages of the game: from the start of the game, it keeps
 * the EntitySpawned, WorldState, EntityDestroyed and every copy of PlayerLeft
 * of the server, with each state of the server rebuilt in full from its
 * DeltaWorldState parts by the WorldStateAssembler, and the first copy of each
 * GameEvent (see GameEventHistory), in the order they arrived, until
 * drainEntityMessages() hands them over; leaving the session drops those not
 * handed over and forgets the events seen. The game screen drains them every
 * frame, behind the options too, so they never pile up for longer than the
 * loading of the game.
 *
 * The game starts with a GameStarted, or with a WorldState or a
 * DeltaWorldState when the GameStarted was lost: it tells the waiting room. The
 * first GameEnded of a game goes to the game screen with the messages, and the
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
   * @param reminder Told when a WorldState arrives; it must outlive the queue.
   */
  EntityMessageQueue(network::MessageDispatcher& dispatcher, WaitingRoom& room,
                     GameLoadedReminder& reminder);

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
  /** @brief Registers the handlers of the two messages that carry a state: a
   * WorldState is kept as it is, a DeltaWorldState once the assembler rebuilt
   * it. */
  void registerStateHandlers(network::MessageDispatcher& dispatcher);
  /** @brief Keeps @p message until the next drain, once the game started. */
  void keep(EntityMessage message);

  WaitingRoom* room_;
  GameLoadedReminder* reminder_;
  std::vector<EntityMessage> messages_;
  GameEventHistory gameEvents_;
  EnemyActionHistory enemyActions_;
  WorldStateAssembler assembler_;
};

}  // namespace rtype::client
