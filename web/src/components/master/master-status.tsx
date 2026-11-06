import { CircleCheck, CircleX, type LucideIcon } from 'lucide-react';
import { getMasterHealth, type MasterHealth } from '@/hooks/api';
import { cn } from '@/lib/utils';

const STATUS_LOOKS: Record<
  MasterHealth['state'],
  { icon: LucideIcon; iconClassName: string }
> = {
  online: { icon: CircleCheck, iconClassName: 'text-online' },
  unreachable: { icon: CircleX, iconClassName: 'text-destructive' },
};

function statusTextOf(health: MasterHealth): string {
  switch (health.state) {
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
    <span className="inline-flex items-center gap-2">
      <Icon aria-hidden="true" className={cn('size-4', look.iconClassName)} />
      {statusTextOf(health)}
    </span>
  );
}

/** The health of the master as the server read it for this request. */
export async function MasterStatus() {
  const health = await getMasterHealth();

  return (
    <p role="status" className="text-sm">
      <MasterStatusText health={health} />
    </p>
  );
}
