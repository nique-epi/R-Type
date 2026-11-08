import type * as React from 'react';
import { cn } from '@/lib/utils';

export type StatProps = React.ComponentProps<'div'> & {
  /** The figure: a number, or a <CountUp> that climbs to it. */
  value: React.ReactNode;
  /** What the figure counts, so it is never read alone. */
  label: React.ReactNode;
};

/** A figure in pixel digits over the words of what it counts. */
export function Stat({ value, label, className, ...props }: StatProps) {
  return (
    <div data-slot="stat" className={cn('grid gap-2', className)} {...props}>
      <span className="font-pixel text-[32px] text-star-white leading-8">
        {value}
      </span>
      <span className="small-caps text-pale-blue">{label}</span>
    </div>
  );
}
