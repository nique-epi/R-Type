'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { cn } from '@/lib/utils';

export type TabItem<Value extends string> = {
  value: Value;
  label: React.ReactNode;
  /** The id of the panel the tab shows, for `aria-controls`. */
  panelId?: string;
};

export type TabsProps<Value extends string> = {
  items: readonly TabItem<Value>[];
  value: Value;
  onValueChange: (value: Value) => void;
  'aria-label': string;
  className?: string;
};

const KEY_STEPS: Readonly<Record<string, number>> = {
  ArrowRight: 1,
  ArrowDown: 1,
  ArrowLeft: -1,
  ArrowUp: -1,
};

/**
 * A row of tabs in one rounded track, the open one filled in cyan. The tabs
 * follow the ARIA tabs pattern: one tab in the Tab order, arrows move to the
 * next one and open it, Home and End go to the ends.
 */
export function Tabs<Value extends string>({
  items,
  value,
  onValueChange,
  className,
  'aria-label': label,
}: TabsProps<Value>) {
  const tabs = useRef<(HTMLButtonElement | null)[]>([]);

  function openAt(index: number) {
    const item = items[(index + items.length) % items.length];
    if (item === undefined) {
      return;
    }
    onValueChange(item.value);
    tabs.current[items.indexOf(item)]?.focus();
  }

  function onKeyDown(event: React.KeyboardEvent, index: number) {
    const step = KEY_STEPS[event.key];
    if (step !== undefined) {
      openAt(index + step);
    } else if (event.key === 'Home') {
      openAt(0);
    } else if (event.key === 'End') {
      openAt(items.length - 1);
    } else {
      return;
    }
    event.preventDefault();
  }

  return (
    <div
      role="tablist"
      aria-label={label}
      data-slot="tabs"
      className={cn(
        'inline-flex w-max max-w-full flex-wrap gap-1.5 rounded-full bg-deep-blue/40 p-1.5',
        className,
      )}
    >
      {items.map((item, index) => {
        const open = item.value === value;
        return (
          <button
            key={item.value}
            ref={element => {
              tabs.current[index] = element;
            }}
            type="button"
            role="tab"
            aria-selected={open}
            aria-controls={item.panelId}
            tabIndex={open ? 0 : -1}
            onClick={() => onValueChange(item.value)}
            onKeyDown={event => onKeyDown(event, index)}
            className={cn(
              'cursor-pointer rounded-full px-[18px] py-3 font-pixel text-xs uppercase leading-3 transition-colors duration-200',
              open
                ? 'bg-hud-cyan text-ink'
                : 'text-pale-blue hover:text-star-white',
            )}
          >
            {item.label}
          </button>
        );
      })}
    </div>
  );
}
