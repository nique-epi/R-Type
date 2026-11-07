'use client';

import type * as React from 'react';
import { useEffect, useRef, useState } from 'react';
import { CutFrame } from '@/components/game/cut-frame';
import { GameChip } from '@/components/game/game-chip';
import { GAME_SCREEN_HEIGHT } from '@/components/game/game-screen';
import { GameText } from '@/components/game/game-text';
import { format } from '@/i18n/format';
import { cn } from '@/lib/utils';

/** A setting of the leaderboard, picked among its options. */
export type RankingFilter = {
  id: string;
  label: string;
  options: readonly string[];
  /** The index of the option in force. */
  index: number;
};

export type RankingFilterLabels = {
  title: string;
  back: string;
  apply: string;
  /** "{label} précédent", "{label} suivant": the names of the arrows. */
  previous: string;
  next: string;
};

const FIRST_ROW_TOP = 68;
const ROW_STEP = 36;
const FOOT_HEIGHT = 72;

/** The height of the dialog for that many settings, from its title to its buttons. */
export function filterDialogHeight(rows: number): number {
  return FIRST_ROW_TOP + rows * ROW_STEP + FOOT_HEIGHT;
}

export type RankingFilterDialogProps = {
  filters: readonly RankingFilter[];
  onApply: (indices: readonly number[]) => void;
  onCancel: () => void;
  labels: RankingFilterLabels;
};

/**
 * The filter of the leaderboard, over the dimmed screen: a row per setting,
 * stepped with the arrows beside its value. Up and Down choose the row,
 * Left and Right step it, Enter applies, Escape goes back unchanged.
 */
export function RankingFilterDialog({
  filters,
  onApply,
  onCancel,
  labels,
}: RankingFilterDialogProps) {
  const [draft, setDraft] = useState(() => filters.map(filter => filter.index));
  const [row, setRow] = useState(0);
  const frame = useRef<HTMLDivElement>(null);
  const height = filterDialogHeight(filters.length);

  useEffect(() => {
    frame.current?.focus();
  }, []);

  function step(at: number, by: number) {
    const filter = filters[at];
    if (filter === undefined) {
      return;
    }
    setDraft(current =>
      current.map((value, index) =>
        index === at
          ? Math.max(0, Math.min(filter.options.length - 1, value + by))
          : value,
      ),
    );
  }

  function applyIfChanged() {
    if (draft.some((value, index) => value !== filters[index]?.index)) {
      onApply(draft);
    } else {
      onCancel();
    }
  }

  function onKeyDown(event: React.KeyboardEvent) {
    switch (event.key) {
      case 'ArrowUp':
        setRow(Math.max(0, row - 1));
        break;
      case 'ArrowDown':
        setRow(Math.min(filters.length - 1, row + 1));
        break;
      case 'ArrowLeft':
        step(row, -1);
        break;
      case 'ArrowRight':
        step(row, 1);
        break;
      case 'Enter':
        applyIfChanged();
        break;
      case 'Escape':
        onCancel();
        break;
      default:
        return;
    }
    event.preventDefault();
    event.stopPropagation();
  }

  return (
    <>
      <div aria-hidden="true" className="absolute inset-0 z-5 bg-sky/72" />
      <CutFrame
        ref={frame}
        role="dialog"
        aria-label={labels.title}
        tabIndex={-1}
        onKeyDown={onKeyDown}
        className="absolute left-40 z-6 w-160"
        style={{ top: Math.round((GAME_SCREEN_HEIGHT - height) / 2), height }}
      >
        <GameText className="absolute top-6 left-8 text-hud-cyan">
          {labels.title}
        </GameText>
        {filters.map((filter, index) => {
          const value = draft[index] ?? filter.index;
          return (
            <div
              key={filter.id}
              className="absolute inset-x-0 h-6"
              style={{ top: FIRST_ROW_TOP + index * ROW_STEP }}
            >
              <GameText
                as="span"
                className={cn(
                  'absolute top-1 left-8',
                  index === row ? 'text-hud-cyan' : 'text-pale-blue',
                )}
              >
                {filter.label}
              </GameText>
              <StepArrow
                className="left-59"
                disabled={value <= 0}
                aria-label={format(labels.previous, { label: filter.label })}
                onClick={() => step(index, -1)}
              >
                {'<'}
              </StepArrow>
              <GameText
                as="span"
                className="absolute left-66 grid h-6 w-76 place-items-center bg-ink text-star-white shadow-sunk"
              >
                {filter.options[value]}
              </GameText>
              <StepArrow
                className="left-143"
                disabled={value >= filter.options.length - 1}
                aria-label={format(labels.next, { label: filter.label })}
                onClick={() => step(index, 1)}
              >
                {'>'}
              </StepArrow>
            </div>
          );
        })}
        <div className="absolute inset-x-0 bottom-6 flex justify-center gap-8">
          <GameChip className="h-10 px-6" onClick={onCancel}>
            {labels.back}
          </GameChip>
          <GameChip className="h-10 px-6" selected onClick={applyIfChanged}>
            {labels.apply}
          </GameChip>
        </div>
      </CutFrame>
    </>
  );
}

function StepArrow({ className, ...props }: React.ComponentProps<'button'>) {
  return (
    <button
      type="button"
      className={cn(
        'absolute top-0 size-6 cursor-pointer font-pixel text-base text-hud-cyan disabled:cursor-default disabled:text-deep-blue',
        className,
      )}
      {...props}
    />
  );
}
