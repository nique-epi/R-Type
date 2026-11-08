'use client';

import type * as React from 'react';
import { useRef, useState } from 'react';
import { CutFrame } from '@/components/game/cut-frame';
import { GameChip } from '@/components/game/game-chip';
import { GameScreen } from '@/components/game/game-screen';
import { GameText } from '@/components/game/game-text';
import { CountUp } from '@/components/motion/count-up';
import { Keycap } from '@/components/primitives/text/keycap';
import { StarLayers } from '@/components/sky/star-layers';
import { SHEETS } from '@/components/sprites/sheets';
import { Sprite } from '@/components/sprites/sprite';
import { useFullscreen } from '@/hooks/use-fullscreen';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';
import { runShortcut } from '@/lib/shortcuts';
import { cn } from '@/lib/utils';
import {
  type RankingFilter,
  RankingFilterDialog,
  type RankingFilterLabels,
} from './ranking-filter-dialog';

/** A pilot of the leaderboard. */
export type RankingRow = {
  rank: number;
  name: string;
  score: number;
  /** "12 NOV 26". */
  date: string;
};

export type RankingScreenLabels = {
  screen: string;
  title: string;
  scope: string;
  previousTab: string;
  nextTab: string;
  filterButton: string;
  back: string;
  /** The chip that shows the screen fullscreen, then the one that leaves it. */
  fullscreen: string;
  exitFullscreen: string;
  keys: {
    previousTab: string;
    nextTab: string;
    filter: string;
    fullscreen: string;
    back: string;
  };
  filter: RankingFilterLabels;
};

export type RankingScreenProps = {
  /** The two scopes of the leaderboard: this server, everyone. */
  tabs: readonly [string, string];
  tab: number;
  onTabChange: (tab: number) => void;
  /** The tabs that cannot be opened, such as the one of a server while the master lists none. */
  disabledTabs?: readonly number[];
  rows: readonly RankingRow[];
  /** What the list shows, under it: "NEBULA LYON · PARTIES NORMALES, 2 PILOTES". */
  summary: React.ReactNode;
  /** What the frame says while it holds no pilot: nobody has played those games, or the master does not answer. */
  empty: React.ReactNode;
  filters: readonly RankingFilter[];
  onApplyFilters: (indices: readonly number[]) => void;
  onBack: () => void;
  labels: RankingScreenLabels;
};

const TAB_LEFT = 196;
const TAB_STEP = 288;
const ROW_TOP = 14;
const ROW_STEP = 26;
/** The first three ranks are drawn in white, white, then bronze, as on the podium of the client. */
const PODIUM_COLORS = [
  'text-star-white',
  'text-star-white',
  'text-bronze',
] as const;

/**
 * The leaderboard screen of the client, in the page: two flags and two
 * guards around the title, a tab per scope (Q and E), the ten best pilots
 * whose scores count up as they slide in, and the filter (F).
 */
export function RankingScreen({
  tabs,
  tab,
  onTabChange,
  disabledTabs = [],
  rows,
  summary,
  empty,
  filters,
  onApplyFilters,
  onBack,
  labels,
}: RankingScreenProps) {
  const root = useRef<HTMLDivElement>(null);
  const first = useRef(true);
  const [filterOpen, setFilterOpen] = useState(false);
  const fullscreen = useFullscreen(root);

  useMotion(
    () => {
      const entering = first.current;
      first.current = false;
      if (entering) {
        gsap.from('[data-ranking-title]', {
          y: -48,
          opacity: 0,
          duration: 0.5,
          ease: 'steps(6)',
        });
        gsap.from('[data-flag]', {
          y: -200,
          duration: 0.6,
          ease: 'steps(8)',
          stagger: 0.15,
        });
        gsap.from('[data-guard]', {
          opacity: 0,
          duration: 0.3,
          ease: 'steps(3)',
          delay: 0.4,
        });
      }
      gsap.from('[data-ranking-row]', {
        x: -32,
        opacity: 0,
        duration: 0.18,
        ease: 'steps(3)',
        stagger: 0.07,
        delay: entering ? 0.4 : 0,
      });
    },
    { scope: root, dependencies: [rows] },
  );

  function changeTab(index: number) {
    if (!disabledTabs.includes(index)) {
      onTabChange(index);
    }
  }

  function onKeyDown(event: React.KeyboardEvent<HTMLDivElement>) {
    if (filterOpen) {
      return;
    }
    runShortcut(event, {
      q: () => changeTab(0),
      e: () => changeTab(1),
      f: () => setFilterOpen(true),
      z: fullscreen.toggle,
      escape: onBack,
    });
  }

  return (
    <div
      ref={root}
      className={cn(
        fullscreen.active &&
          'relative isolate grid h-screen place-items-center bg-sky',
      )}
    >
      {fullscreen.active ? <StarLayers className="-z-10" /> : null}
      <GameScreen
        aria-label={labels.screen}
        onKeyDown={onKeyDown}
        fitHeight={fullscreen.active}
        upscale={fullscreen.active}
        settle
      >
        <Sprite data-flag {...FLAG} className="absolute top-0 left-4" />
        <Sprite
          data-flag
          {...FLAG}
          animation={{ ...FLAG.animation, delay: -0.65 }}
          className="absolute top-0 left-210"
        />
        <div className="absolute inset-x-0 top-4 flex items-start justify-center gap-6">
          <Sprite data-guard {...GUARD} />
          <GameText
            data-ranking-title
            outlined
            className="mt-5 text-center text-[32px] text-star-white leading-8"
          >
            {labels.title}
          </GameText>
          <Sprite
            data-guard
            {...GUARD}
            frame={{ ...GUARD.frame, x: GUARD_FLIPPED_X }}
            animation={{ ...GUARD.animation, delay: -0.125 }}
          />
        </div>
        <span className="absolute top-20 left-40 h-0.5 w-160 bg-edge-blue" />

        <button
          type="button"
          aria-label={labels.previousTab}
          onClick={() => changeTab(0)}
          className="absolute top-26 left-40 cursor-pointer"
        >
          <Keycap size="sm">{labels.keys.previousTab}</Keycap>
        </button>
        <button
          type="button"
          aria-label={labels.nextTab}
          onClick={() => changeTab(1)}
          className="absolute top-26 left-193 cursor-pointer"
        >
          <Keycap size="sm">{labels.keys.nextTab}</Keycap>
        </button>
        <div role="tablist" aria-label={labels.scope}>
          {tabs.map((label, index) => {
            const open = index === tab;
            return (
              <button
                key={label}
                type="button"
                role="tab"
                aria-selected={open}
                disabled={disabledTabs.includes(index)}
                onClick={() => changeTab(index)}
                className={cn(
                  'cut-top-8 absolute top-25 h-8 w-70 cursor-pointer bg-edge-blue disabled:cursor-not-allowed disabled:opacity-40',
                  open && 'h-[34px] bg-hud-cyan',
                )}
                style={{ left: TAB_LEFT + index * TAB_STEP }}
              >
                <span
                  className={cn(
                    'cut-top-6 absolute inset-x-0.5 top-0.5 bottom-0 grid place-items-center bg-sky font-pixel text-base uppercase leading-4',
                    open ? 'text-hud-cyan' : 'text-pale-blue',
                  )}
                >
                  {label}
                </span>
              </button>
            );
          })}
        </div>

        <CutFrame className="absolute top-33 left-40 h-75 w-160">
          {rows.length === 0 ? (
            <div className="absolute inset-0 grid place-items-center px-8 text-center">
              <GameText face="small" className="text-pale-blue">
                {empty}
              </GameText>
            </div>
          ) : null}
          {rows.map((row, index) => (
            // A tie shares its rank, so the key adds the player and the day.
            <div
              key={`${row.rank}-${row.name}-${row.date}`}
              data-ranking-row
              className="absolute left-6 grid w-148 grid-cols-[48px_minmax(0,1fr)_150px_116px] items-center gap-x-5"
              style={{ top: ROW_TOP + index * ROW_STEP }}
            >
              <GameText
                as="span"
                className={cn(
                  'text-right',
                  PODIUM_COLORS[row.rank - 1] ?? 'text-pale-blue',
                )}
              >
                {row.rank}
              </GameText>
              <GameText as="span" className="truncate text-star-white">
                {row.name}
              </GameText>
              <GameText as="span" className="text-right text-star-white">
                <CountUp
                  value={row.score}
                  duration={0.6}
                  delay={0.4 + index * 0.07}
                  ease="steps(20)"
                />
              </GameText>
              <GameText
                as="span"
                face="small"
                className="text-right text-pale-blue"
              >
                {row.date}
              </GameText>
            </div>
          ))}
        </CutFrame>
        <span
          aria-hidden="true"
          className="absolute top-33 h-0.5 w-69 bg-sky"
          style={{ left: TAB_LEFT + 2 + tab * TAB_STEP }}
        />

        <GameText
          outlined
          face="small"
          className="absolute top-122 left-40 max-w-190 text-pale-blue"
        >
          {summary}
        </GameText>
        <div className="absolute top-113 right-40 flex gap-4">
          <GameChip
            shortcut={labels.keys.filter}
            onClick={() => setFilterOpen(true)}
          >
            {labels.filterButton}
          </GameChip>
          <GameChip
            shortcut={labels.keys.fullscreen}
            onClick={fullscreen.toggle}
          >
            {fullscreen.active ? labels.exitFullscreen : labels.fullscreen}
          </GameChip>
          <GameChip shortcut={labels.keys.back} onClick={onBack}>
            {labels.back}
          </GameChip>
        </div>

        {filterOpen ? (
          <RankingFilterDialog
            filters={filters}
            labels={labels.filter}
            onCancel={() => setFilterOpen(false)}
            onApply={indices => {
              setFilterOpen(false);
              onApplyFilters(indices);
            }}
          />
        ) : null}
      </GameScreen>
    </div>
  );
}

/** A flag of the leaderboard, 52 × 99 drawn at ×2, waving in 32 frames. */
const FLAG = {
  sheet: SHEETS.flag,
  frame: { x: 0, y: 0, width: 52, height: 99 },
  scale: 2,
  animation: { frames: 32, seconds: 1.6 },
};

/** A guard of the leaderboard, 28 × 30 drawn at ×2, stepping in place in 2 frames. */
const GUARD = {
  sheet: SHEETS.guard,
  frame: { x: 0, y: 0, width: 28, height: 30 },
  scale: 2,
  animation: { frames: 2, seconds: 0.5 },
};

/** guard.png holds two frames facing right, then the same two facing left. */
const GUARD_FLIPPED_X = 56;
