import Link from 'next/link';
import type * as React from 'react';
import { Triangle } from '@/components/primitives/icons/triangle';

export type ExploreTileProps = {
  href: string;
  label: string;
  title: string;
  /** The word of the arrow at the foot of the tile: "Voir". */
  more: string;
};

/** A tile that leads to another part of the site; it lifts on hover. */
export function ExploreTile({ href, label, title, more }: ExploreTileProps) {
  return (
    <Link
      href={href}
      data-slot="explore-tile"
      className="group/tile grid content-start gap-3 rounded-3xl bg-deep-blue/35 p-6 text-left text-pale-blue no-underline transition duration-250 hover:-translate-y-1 hover:bg-deep-blue/70 focus-visible:-translate-y-1 focus-visible:bg-deep-blue/70"
    >
      <span className="small-caps text-star-white">{label}</span>
      <span className="font-pixel text-base text-star-white uppercase leading-6">
        {title}
      </span>
      <span className="inline-flex items-center gap-3 font-pixel text-hud-cyan text-xs uppercase leading-4">
        {more}
        <Triangle className="transition-transform duration-250 group-hover/tile:translate-x-1.5" />
      </span>
    </Link>
  );
}

/** Four tiles in a row, two on a tablet, one on a phone. */
export function ExploreTiles({ children }: { children: React.ReactNode }) {
  return (
    <div className="mt-14 grid grid-cols-4 gap-6 max-md:grid-cols-2 max-sm:grid-cols-1">
      {children}
    </div>
  );
}
