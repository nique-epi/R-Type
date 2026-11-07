import Link from 'next/link';
import type * as React from 'react';
import { Triangle } from '@/components/primitives/icons/triangle';
import { StatusBadge } from '@/components/primitives/status/status-badge';
import type { ServerStatus } from '@/lib/rtype/servers';

export type ServerRowProps = {
  href: string;
  name: string;
  status: ServerStatus;
  statusLabel: string;
  /** What is left on it: "2 places libres · 6 pilotes", the counts in white. */
  seats: React.ReactNode;
  /** The name of the link for a screen reader: "Nebula Lyon, voir le serveur". */
  'aria-label': string;
};

/** A server in a list of the home page: its name, its status, its free seats, and a link to it. */
export function ServerRow({
  href,
  name,
  status,
  statusLabel,
  seats,
  'aria-label': label,
}: ServerRowProps) {
  return (
    <Link
      href={href}
      aria-label={label}
      data-slot="server-row"
      className="row-link group/row grid w-full grid-cols-[minmax(0,1.4fr)_auto_minmax(0,1fr)_24px] items-center gap-6 px-6 py-5 text-left text-pale-blue max-md:grid-cols-[1fr_auto] max-md:gap-x-4 max-md:gap-y-2"
    >
      <span className="font-pixel text-base text-hud-cyan uppercase leading-6">
        {name}
      </span>
      <StatusBadge status={status}>{statusLabel}</StatusBadge>
      <span className="small-caps [&_b]:font-normal [&_b]:text-star-white">
        {seats}
      </span>
      <Triangle className="text-hud-cyan transition-transform duration-250 group-hover/row:translate-x-2 group-focus-visible/row:translate-x-2 max-md:hidden" />
    </Link>
  );
}
