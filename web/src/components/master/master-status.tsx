'use client';

import {
  CircleCheck,
  CircleX,
  LoaderCircle,
  type LucideIcon,
} from 'lucide-react';
import { type MasterHealth, useMasterHealth } from '@/hooks/api';
import { cn } from '@/lib/utils';

const STATUS_LOOKS: Record<
  MasterHealth['state'],
  { icon: LucideIcon; iconClassName: string; textClassName: string }
> = {
  checking: {
    icon: LoaderCircle,
    iconClassName: 'animate-spin motion-reduce:animate-none',
    textClassName: 'text-muted-foreground',
  },
  online: {
    icon: CircleCheck,
    iconClassName: 'text-online',
    textClassName: '',
  },
  unreachable: {
    icon: CircleX,
    iconClassName: 'text-destructive',
    textClassName: '',
  },
};

function statusTextOf(health: MasterHealth): string {
  switch (health.state) {
    case 'checking':
      return 'Checking the master…';
    case 'online':
      return `Online, version ${health.version}`;
    case 'unreachable':
      return 'Unreachable';
  }
}

/** The health of the master in words, with an icon that only repeats them. */
export function MasterStatusText({ health }: { health: MasterHealth }) {
  const look = STATUS_LOOKS[health.state];
  const Icon = look.icon;

  return (
    <span className={cn('inline-flex items-center gap-2', look.textClassName)}>
      <Icon aria-hidden="true" className={cn('size-4', look.iconClassName)} />
      {statusTextOf(health)}
    </span>
  );
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
