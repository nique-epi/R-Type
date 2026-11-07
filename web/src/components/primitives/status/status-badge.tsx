import type * as React from 'react';
import type { ServerStatus } from '@/lib/rtype/servers';
import { cn } from '@/lib/utils';

const STATUS_LOOKS: Record<ServerStatus, { text: string; marker: string }> = {
  online: { text: 'text-player-light-green', marker: 'bg-status-online' },
  degraded: { text: 'text-star-gold', marker: 'bg-status-degraded' },
  offline: {
    text: 'text-pale-blue',
    marker: 'border-2 border-status-offline bg-transparent',
  },
};

export type StatusBadgeProps = React.ComponentProps<'span'> & {
  status: ServerStatus;
  /** The word of the status: the marker only repeats it. */
  children: React.ReactNode;
};

/** The status of a server: a square marker and its word, never the color alone. */
export function StatusBadge({
  status,
  className,
  children,
  ...props
}: StatusBadgeProps) {
  const look = STATUS_LOOKS[status];
  return (
    <span
      data-slot="status-badge"
      data-status={status}
      className={cn(
        'inline-flex items-center gap-2 whitespace-nowrap small-caps',
        look.text,
        className,
      )}
      {...props}
    >
      <span
        aria-hidden="true"
        className={cn('box-border size-2.5', look.marker)}
      />
      {children}
    </span>
  );
}
