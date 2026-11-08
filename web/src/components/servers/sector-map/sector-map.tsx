'use client';

import type * as React from 'react';
import { useRef, useState } from 'react';
import { GameChip } from '@/components/game/game-chip';
import { GameScreen } from '@/components/game/game-screen';
import { GameText } from '@/components/game/game-text';
import { ScreenCorners } from '@/components/game/screen-corners';
import { useMotion } from '@/hooks/use-motion';
import { format, formatCount } from '@/i18n/format';
import type { Locale } from '@/i18n/locales';
import { gsap } from '@/lib/gsap';
import { withoutAccents } from '@/lib/rtype/pixel-text';
import { MAXIMUM_PILOTS_PER_GAME } from '@/lib/rtype/players';
import { canJoin, pilotsOf } from '@/lib/rtype/server-seats';
import { runShortcut, type Shortcuts } from '@/lib/shortcuts';
import {
  angleOf,
  type Bearing,
  CLOSE_TARGET,
  CLOSE_ZOOM,
  isOnMap,
  placeOf,
  toMapPixels,
  toMinimapPixels,
  viewOn,
  WHOLE_VIEW,
} from './geometry';
import { cycleServer, quickPlayTarget, serversOnMap, starLook } from './rules';
import { type BubbleLine, SectorBubble } from './sector-bubble';
import {
  PANEL_GAMES,
  type PanelGame,
  SectorGamesPanel,
} from './sector-games-panel';
import { SectorMinimap } from './sector-minimap';
import { SectorSky } from './sector-sky';
import { SectorStar, StarLock } from './sector-star';
import type { HeldSeat, MapServer, MapStage, SectorMapLabels } from './types';

export type SectorMapProps = {
  servers: readonly MapServer[];
  /** The seat the reader holds, so the map shows it and offers to leave it. */
  held?: HeldSeat | null;
  /**
   * Joins a game. Without it the map only shows the servers and their games:
   * it offers no quick play, no Join and no seat to leave.
   */
  onJoin?: (serverId: string, gameIndex: number) => void;
  /** Leaves the seat held; without it the map offers no way to leave it. */
  onLeave?: () => void;
  onOpenServer: (serverId: string) => void;
  onLeaderboard: () => void;
  /** Escape on the whole sky: leave the map. */
  onExit: () => void;
  labels: SectorMapLabels;
  locale: Locale;
};

function bearingOf(server: MapServer): Bearing {
  return { angle: angleOf(server.number, server.ping), ping: server.ping };
}

/**
 * The sector map of the client, in the page: the planet of the reader and
 * the servers around it, further away the higher their ping. A click or
 * Enter on a star zooms on it, then lists its games; Escape steps back.
 * The arrows move between servers (between games once listed), Q joins the
 * nearest free seat, L and H open the leaderboard and the hosting form. A map
 * given no `onJoin` is read only: Q and Join are gone.
 */
export function SectorMap({
  servers,
  held = null,
  onJoin,
  onLeave,
  onOpenServer,
  onLeaderboard,
  onExit,
  labels,
  locale,
}: SectorMapProps) {
  const root = useRef<HTMLDivElement>(null);
  const previous = useRef<{ stage: MapStage; chosenId: string | null } | null>(
    null,
  );
  const [chosenId, setChosenId] = useState<string | null>(null);
  const [requestedStage, setStage] = useState<MapStage>('whole');
  const [gameIndex, setGameIndex] = useState(0);

  const onMap = serversOnMap(servers);
  const chosen = onMap.find(server => server.id === chosenId) ?? null;
  const stage: MapStage = chosen ? requestedStage : 'whole';
  const view =
    stage === 'whole' || chosen === null
      ? WHOLE_VIEW
      : viewOn(bearingOf(chosen), CLOSE_ZOOM, CLOSE_TARGET);
  /** Each server with its place in the view and on the whole sky, computed once a render. */
  const placed = onMap.map(server => {
    const bearing = bearingOf(server);
    return {
      server,
      place: toMapPixels(placeOf(bearing, view)),
      onWholeSky: placeOf(bearing, WHOLE_VIEW),
    };
  });
  const chosenPlace =
    placed.find(entry => entry.server === chosen)?.place ?? null;
  const chosenGame = chosen?.games[gameIndex];
  const heldHere =
    held !== null && chosen !== null && held.serverId === chosen.id;
  const canJoinChosen =
    chosen !== null &&
    chosenGame !== undefined &&
    canJoin(chosen, chosenGame) &&
    held === null &&
    onJoin !== undefined;

  function showGames(server: MapServer) {
    setChosenId(server.id);
    setStage('games');
    setGameIndex(0);
  }

  function joinChosen() {
    if (chosen && canJoinChosen) {
      onJoin?.(chosen.id, gameIndex);
    }
  }

  function advance() {
    if (chosen === null) {
      setChosenId(onMap[0]?.id ?? null);
    } else if (stage === 'whole') {
      setStage('close');
    } else if (stage === 'close') {
      showGames(chosen);
    } else {
      joinChosen();
    }
  }

  function back() {
    if (stage === 'games') {
      setStage('close');
    } else if (stage === 'close') {
      setStage('whole');
    } else {
      onExit();
    }
  }

  function cycle(step: number) {
    if (stage === 'games' && chosen) {
      setGameIndex(
        Math.max(
          0,
          Math.min(
            Math.min(chosen.games.length, PANEL_GAMES) - 1,
            gameIndex + step,
          ),
        ),
      );
      return;
    }
    const next = cycleServer(onMap, chosenId, step);
    if (next) {
      setChosenId(next.id);
    }
  }

  function pickStar(server: MapServer) {
    if (server.id !== chosenId) {
      setChosenId(server.id);
      setStage(stage === 'games' ? 'close' : stage);
    } else if (stage === 'whole') {
      setStage('close');
    } else if (stage === 'close') {
      showGames(server);
    }
  }

  function pickGame(index: number) {
    if (index === gameIndex) {
      joinChosen();
    } else {
      setGameIndex(index);
    }
  }

  function quickPlay() {
    if (held) {
      const server = onMap.find(candidate => candidate.id === held.serverId);
      if (server) {
        showGames(server);
        setGameIndex(held.gameIndex);
      }
      return;
    }
    const target = quickPlayTarget(servers);
    if (target && onJoin) {
      showGames(target.server);
      setGameIndex(target.gameIndex);
      onJoin(target.server.id, target.gameIndex);
    }
  }

  const shortcuts: Shortcuts = {
    escape: back,
    ...(onJoin ? { q: quickPlay } : {}),
    l: onLeaderboard,
    ...(stage === 'games' && chosen
      ? {
          i: () => onOpenServer(chosen.id),
          ...(heldHere && onLeave ? { x: onLeave } : {}),
        }
      : {}),
  };

  /** Enter and the arrows act only on the screen itself, never on a button inside it. */
  const screenKeys: Shortcuts = {
    enter: advance,
    arrowright: () => cycle(1),
    arrowdown: () => cycle(1),
    arrowleft: () => cycle(-1),
    arrowup: () => cycle(-1),
  };

  function onKeyDown(event: React.KeyboardEvent<HTMLDivElement>) {
    const onScreen = event.target === event.currentTarget;
    runShortcut(event, onScreen ? { ...shortcuts, ...screenKeys } : shortcuts);
  }

  useMotion(
    () => {
      const before = previous.current;
      previous.current = { stage, chosenId };
      if (before === null) {
        gsap.from('[data-sector-sky]', {
          opacity: 0,
          duration: 0.5,
          ease: 'steps(5)',
        });
        gsap.to('[data-ping-ring]', {
          strokeDashoffset: -20,
          duration: 2,
          ease: 'steps(10)',
          repeat: -1,
        });
        gsap.from('[data-star]', {
          scale: 0,
          duration: 0.3,
          ease: 'steps(3)',
          stagger: 0.08,
          delay: 0.3,
        });
        gsap.from('[data-star-name], [data-ring-label]', {
          opacity: 0,
          duration: 0.3,
          ease: 'steps(3)',
          delay: 0.6,
        });
        gsap.from('[data-slot=game-chip]', {
          y: 24,
          opacity: 0,
          duration: 0.3,
          ease: 'steps(4)',
          stagger: 0.06,
          delay: 0.4,
        });
        return;
      }
      if (
        before.stage !== stage &&
        stage !== 'games' &&
        before.stage !== 'games'
      ) {
        gsap.from('[data-sector-sky]', {
          scale: stage === 'close' ? 0.6 : 1.6,
          transformOrigin: '50% 50%',
          opacity: 0,
          duration: 0.35,
          ease: 'steps(6)',
        });
        gsap.from('[data-star]', {
          scale: 0,
          duration: 0.25,
          ease: 'steps(3)',
          stagger: 0.04,
          delay: 0.15,
        });
        gsap.from('[data-minimap]', {
          opacity: 0,
          y: 16,
          duration: 0.25,
          ease: 'steps(4)',
          delay: 0.2,
        });
      }
      if (stage === 'games' && before.stage !== 'games') {
        gsap.from('[data-games-panel]', {
          x: 64,
          opacity: 0,
          duration: 0.3,
          ease: 'steps(6)',
        });
        gsap.from('[data-game-row]', {
          x: -24,
          opacity: 0,
          duration: 0.2,
          ease: 'steps(4)',
          stagger: 0.06,
          delay: 0.15,
        });
      }
      if (stage !== 'games') {
        gsap.from('[data-star-lock]', {
          scale: 2.4,
          opacity: 0,
          duration: 0.2,
          ease: 'steps(4)',
        });
        gsap.from('[data-bubble]', {
          scaleY: 0,
          transformOrigin: '50% 0%',
          duration: 0.2,
          ease: 'steps(4)',
          delay: 0.05,
        });
      }
    },
    { scope: root, dependencies: [stage, chosenId] },
  );

  const subtitle = format(labels.subtitle, {
    servers: formatCount(onMap.length, labels.servers, locale),
  });

  return (
    <div ref={root}>
      <GameScreen
        aria-label={labels.screen}
        onKeyDown={onKeyDown}
        fitHeight
        upscale
        className="starfield"
      >
        <GameText outlined className="absolute top-12 left-12 text-hud-cyan">
          {labels.title}
        </GameText>
        <GameText
          outlined
          face="small"
          className="absolute top-[74px] left-12 text-pale-blue"
        >
          {subtitle}
        </GameText>

        <div className="absolute top-26 left-12 h-84 w-216 overflow-hidden">
          <SectorSky view={view} ringLabel={labels.ring} />
          {placed.map(({ server, place }) => {
            if (!isOnMap(place)) {
              return null;
            }
            const look = starLook(server);
            const isChosen = server.id === chosenId;
            const aria = format(labels.star, {
              name: server.name,
              pilots: pilotsOf(server),
              ping: server.ping,
            });
            return (
              <SectorStar
                key={server.id}
                place={place}
                name={server.name}
                mark={look.mark}
                tone={look.tone}
                chosen={isChosen}
                aria-label={isChosen ? `${aria}${labels.starChosen}` : aria}
                onPick={() => pickStar(server)}
              />
            );
          })}
          {chosen &&
          chosenPlace &&
          isOnMap(chosenPlace) &&
          stage !== 'games' ? (
            <>
              <StarLock place={chosenPlace} />
              <SectorBubble
                place={chosenPlace}
                title={withoutAccents(chosen.name)}
                lines={bubbleLines(chosen, stage, labels)}
              />
            </>
          ) : null}
          {stage !== 'whole' ? (
            <SectorMinimap
              view={view}
              stars={placed.map(({ server, onWholeSky }) => ({
                id: server.id,
                place: toMinimapPixels(onWholeSky),
              }))}
            />
          ) : null}
          <ScreenCorners />
          {stage === 'games' && chosen ? (
            <SectorGamesPanel
              title={withoutAccents(chosen.name)}
              aria-label={format(labels.gamesOf, { name: chosen.name })}
              columns={{
                game: labels.columnGame,
                pilots: labels.columnPilots,
                state: labels.columnState,
              }}
              games={chosen.games.map((game, index) => ({
                ...panelGame(chosen, game, index, labels),
                joinable: onJoin !== undefined && canJoin(chosen, game),
              }))}
              chosen={gameIndex}
              onPick={pickGame}
              {...(onJoin
                ? gameDescription(chosen, gameIndex, held, labels)
                : { description: '', descriptionTone: 'plain' as const })}
              noGames={labels.noGames}
            />
          ) : null}
        </div>

        <div className="absolute top-113 right-12 left-12 flex justify-between gap-4">
          {onJoin ? (
            <GameChip shortcut={labels.keys.quickPlay} onClick={quickPlay}>
              {labels.quickPlay}
            </GameChip>
          ) : (
            <span />
          )}
          <div className="flex gap-4">
            {stage === 'games' && chosen ? (
              <>
                {heldHere && onLeave ? (
                  <GameChip shortcut={labels.keys.leave} onClick={onLeave}>
                    {labels.leave}
                  </GameChip>
                ) : null}
                {!heldHere && onJoin ? (
                  <GameChip
                    shortcut={labels.keys.join}
                    disabled={!canJoinChosen}
                    onClick={joinChosen}
                  >
                    {labels.join}
                  </GameChip>
                ) : null}
                <GameChip
                  shortcut={labels.keys.sheet}
                  onClick={() => onOpenServer(chosen.id)}
                >
                  {labels.sheet}
                </GameChip>
              </>
            ) : (
              <>
                <GameChip
                  shortcut={labels.keys.leaderboard}
                  onClick={onLeaderboard}
                >
                  {labels.leaderboard}
                </GameChip>
              </>
            )}
            <GameChip shortcut={labels.keys.back} onClick={back}>
              {labels.back}
            </GameChip>
          </div>
        </div>
      </GameScreen>
    </div>
  );
}

/** The lines of the bubble of the chosen star. */
function bubbleLines(
  server: MapServer,
  stage: MapStage,
  labels: SectorMapLabels,
): BubbleLine[] {
  const lines: BubbleLine[] = [
    {
      text: format(labels.bubblePilots, {
        pilots: pilotsOf(server),
        capacity: server.capacity,
        games: server.games.length,
      }),
      tone: 'plain',
    },
    {
      text: format(labels.bubblePing, {
        ping: server.ping,
        version: server.version.toUpperCase(),
      }),
      tone: 'plain',
    },
  ];
  if (server.status === 'degraded') {
    lines.push({ text: labels.bubbleSlow, tone: 'warning' });
  }
  lines.push({
    text: stage === 'whole' ? labels.bubbleZoom : labels.bubbleGames,
    tone: 'hint',
  });
  return lines;
}

/** A game of the chosen server, as its row of the panel reads. */
function panelGame(
  server: MapServer,
  game: MapServer['games'][number],
  index: number,
  labels: SectorMapLabels,
): PanelGame {
  const name = format(labels.game, { number: index + 1 });
  const state = labels.states[game.state];
  return {
    name,
    pilots: `${game.pilots}/${MAXIMUM_PILOTS_PER_GAME}`,
    state,
    joinable: canJoin(server, game),
    'aria-label': format(labels.gameRow, {
      game: name,
      pilots: game.pilots,
      capacity: MAXIMUM_PILOTS_PER_GAME,
      state: state.toLowerCase(),
    }),
  };
}

/** The line under the games: the reader's seat, what Enter does, or why the game is closed. */
function gameDescription(
  server: MapServer,
  gameIndex: number,
  held: HeldSeat | null,
  labels: SectorMapLabels,
): { description: string; descriptionTone: 'you' | 'hint' | 'plain' } {
  if (held && held.serverId === server.id) {
    return {
      description: format(labels.youAreIn, {
        seat: held.seat,
        game: held.gameIndex + 1,
      }),
      descriptionTone: 'you',
    };
  }
  const game = server.games[gameIndex];
  if (game === undefined) {
    return { description: '', descriptionTone: 'plain' };
  }
  const name = format(labels.game, { number: gameIndex + 1 });
  if (canJoin(server, game)) {
    return {
      description: format(held ? labels.leaveToJoin : labels.pressEnterToJoin, {
        game: name,
      }),
      descriptionTone: 'hint',
    };
  }
  return {
    description: format(
      game.state === 'playing' ? labels.inProgress : labels.full,
      { game: name },
    ),
    descriptionTone: 'plain',
  };
}
