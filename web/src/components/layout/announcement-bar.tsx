import Link from 'next/link';
import type * as React from 'react';

export type AnnouncementBarProps = {
  /** The news: "v0.1.0 disponible depuis le 2 novembre 2026". */
  children: React.ReactNode;
  action: { href: string; label: string };
};

/** The cyan band over the header that announces the last release. */
export function AnnouncementBar({ children, action }: AnnouncementBarProps) {
  return (
    <div
      data-slot="announcement-bar"
      className="flex flex-wrap justify-center gap-3 rounded-b-lg bg-edge-blue px-4 py-2.5 text-center small-caps text-ink"
    >
      <span>{children}</span>
      <Link
        href={action.href}
        className="text-ink underline decoration-2 underline-offset-2"
      >
        {action.label}
      </Link>
    </div>
  );
}
