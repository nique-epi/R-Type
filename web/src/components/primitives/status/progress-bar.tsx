import type * as React from 'react';
import { cn } from '@/lib/utils';

export type ProgressBarProps = Omit<React.ComponentProps<'div'>, 'children'> & {
  /** From 0 to 100; a value outside is drawn at the nearest end. */
  value: number;
  'aria-label': string;
};

const FULL = 100;

/** A rounded track that fills in edge blue, read as a progress bar. */
export function ProgressBar({ value, className, ...props }: ProgressBarProps) {
  const shown = Math.min(FULL, Math.max(0, Math.round(value)));
  return (
    <div
      role="progressbar"
      aria-valuemin={0}
      aria-valuemax={FULL}
      aria-valuenow={shown}
      data-slot="progress-bar"
      className={cn(
        'relative h-7 overflow-hidden rounded-full border-2 border-deep-blue bg-ink/80',
        className,
      )}
      {...props}
    >
      <span
        className="absolute inset-y-1 left-1 rounded-full bg-edge-blue"
        style={{ width: `calc((100% - 8px) * ${shown / FULL})` }}
      />
    </div>
  );
}
