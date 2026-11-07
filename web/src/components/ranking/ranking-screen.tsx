'use client';

import type * as React from 'react';
import { useRef, useState } from 'react';
import { CutFrame } from '@/components/game/cut-frame';
import { GameChip } from '@/components/game/game-chip';
import { GameScreen } from '@/components/game/game-screen';
import { GameText } from '@/components/game/game-text';
import { CountUp } from '@/components/motion/count-up';
import { Keycap } from '@/components/primitives/keycap';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';
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
  keys: { previousTab: string; nextTab: string; filter: string; back: string };
  filter: RankingFilterLabels;
};

export type RankingScreenProps = {
  /** The two scopes of the leaderboard: this server, everyone. */
  tabs: readonly [string, string];
  tab: number;
  onTabChange: (tab: number) => void;
  rows: readonly RankingRow[];
  /** What the list shows, under it: "NEBULA LYON · PARTIES NORMALES, 2 PILOTES". */
  summary: React.ReactNode;
  /** A note at the foot of the screen. */
  footnote?: React.ReactNode;
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
  rows,
  summary,
  footnote,
  filters,
  onApplyFilters,
  onBack,
  labels,
}: RankingScreenProps) {
  const root = useRef<HTMLDivElement>(null);
  const first = useRef(true);
  const [filterOpen, setFilterOpen] = useState(false);

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

  function onKeyDown(event: React.KeyboardEvent<HTMLDivElement>) {
    if (filterOpen) {
      return;
    }
    const actions: Readonly<Record<string, () => void>> = {
      q: () => onTabChange(0),
      e: () => onTabChange(1),
      f: () => setFilterOpen(true),
      escape: onBack,
    };
    const action = actions[event.key.toLowerCase()];
    if (action) {
      event.preventDefault();
      action();
    }
  }

  return (
    <div ref={root}>
      <GameScreen
        aria-label={labels.screen}
        onKeyDown={onKeyDown}
        settle
        className="starfield"
      >
        <span data-flag aria-hidden="true" className={cn(FLAG, 'left-4')} />
        <span
          data-flag
          aria-hidden="true"
          className={cn(FLAG, 'left-210 [animation-delay:-0.65s]')}
        />
        <span
          data-guard
          aria-hidden="true"
          className={cn(GUARD, 'left-73 animate-guard')}
        />
        <span
          data-guard
          aria-hidden="true"
          className={cn(GUARD, 'left-153 animate-guard-flipped')}
        />
        <GameText
          data-ranking-title
          outlined
          className="absolute inset-x-0 top-9 text-center text-[32px] text-star-white leading-8"
        >
          {labels.title}
        </GameText>
        <span className="absolute top-20 left-40 h-0.5 w-160 bg-edge-blue" />

        <button
          type="button"
          aria-label={labels.previousTab}
          onClick={() => onTabChange(0)}
          className="absolute top-26 left-40 cursor-pointer"
        >
          <Keycap size="sm">{labels.keys.previousTab}</Keycap>
        </button>
        <button
          type="button"
          aria-label={labels.nextTab}
          onClick={() => onTabChange(1)}
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
                onClick={() => onTabChange(index)}
                className={cn(
                  'cut-top-8 absolute top-25 h-8 w-70 cursor-pointer bg-edge-blue',
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
          {rows.map((row, index) => (
            <div
              key={row.rank}
              data-ranking-row
              className="absolute left-6 grid w-148 grid-cols-[48px_1fr_150px_116px] items-center gap-x-5"
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
              <GameText as="span" className="whitespace-nowrap text-star-white">
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
          className="absolute top-115 left-40 max-w-95 text-pale-blue"
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
          <GameChip shortcut={labels.keys.back} onClick={onBack}>
            {labels.back}
          </GameChip>
        </div>
        {footnote ? (
          <GameText
            face="small"
            className="absolute inset-x-0 top-127 text-center text-pale-blue"
          >
            {footnote}
          </GameText>
        ) : null}

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

/** A flag of the leaderboard sheet, 52 × 99 drawn at ×2, waving in 32 frames. */
const FLAG =
  'pixelated absolute top-0 h-[198px] w-26 bg-[url(/sprites/flag.png)] bg-no-repeat bg-size-[3328px_198px] animate-flag motion-reduce:animate-none';

/** A guard of the leaderboard sheet, 28 × 30 drawn at ×2, stepping in place. */
const GUARD =
  'pixelated absolute top-4 h-15 w-14 bg-[url(/sprites/guard.png)] bg-no-repeat bg-size-[224px_60px] motion-reduce:animate-none';
