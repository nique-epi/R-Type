'use client';

import { CircleCheck, CircleX, LoaderCircle } from 'lucide-react';
import { type MasterHealth, useMasterHealth } from '@/hooks/api';

/** The health of the master in words, with an icon that only repeats them. */
export function MasterStatusText({ health }: { health: MasterHealth }) {
  switch (health.state) {
    case 'checking':
      return (
        <span className="inline-flex items-center gap-2 text-muted-foreground">
          <LoaderCircle
            aria-hidden="true"
            className="size-4 animate-spin motion-reduce:animate-none"
          />
          Checking the master…
        </span>
      );
    case 'online':
      return (
        <span className="inline-flex items-center gap-2">
          <CircleCheck aria-hidden="true" className="size-4 text-online" />
          Online, version {health.version}
        </span>
      );
    case 'unreachable':
      return (
        <span className="inline-flex items-center gap-2">
          <CircleX aria-hidden="true" className="size-4 text-destructive" />
          Unreachable
        </span>
      );
  }
}

/** The live health of the master, announced to screen readers as it changes. */
export function MasterStatus() {
  const health = useMasterHealth();

  return (
    <p role="status" className="text-sm">
      <MasterStatusText health={health} />
    </p>
  );
}
