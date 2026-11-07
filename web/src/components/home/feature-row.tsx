import type * as React from 'react';
import { Parallax } from '@/components/motion/parallax';
import { Heading } from '@/components/primitives/typography';
import { cn } from '@/lib/utils';

export type FeatureRowProps = {
  /** The background of the art, a class such as `starfield` or `bg-deep-blue`. */
  artClassName?: string;
  /** The sprite in the middle of the art. */
  art: React.ReactNode;
  label: React.ReactNode;
  title: React.ReactNode;
  text: React.ReactNode;
  /** A link under the text: "Voir les serveurs ▸". */
  action?: React.ReactNode;
  /** Puts the art on the right, so rows alternate down the page. */
  reversed?: boolean;
};

/** A feature of the game: a rounded art that drifts with the scroll, and its words beside it. */
export function FeatureRow({
  artClassName,
  art,
  label,
  title,
  text,
  action,
  reversed = false,
}: FeatureRowProps) {
  return (
    <div
      data-slot="feature-row"
      className={cn(
        'flex flex-wrap items-center gap-x-16 gap-y-12 py-14',
        reversed && 'flex-row-reverse',
      )}
    >
      <div className="relative h-[300px] min-w-0 flex-[1_1_420px] overflow-hidden rounded-[32px]">
        <Parallax
          className={cn('absolute -inset-y-[10%] inset-x-0', artClassName)}
        >
          <span className="absolute top-1/2 left-1/2 -translate-1/2">
            {art}
          </span>
        </Parallax>
      </div>
      <div className="grid min-w-0 flex-[1_1_320px] content-center gap-4">
        <p className="m-0 small-caps text-star-white">{label}</p>
        <Heading as="h3" size="title" className="text-star-white">
          {title}
        </Heading>
        <p className="m-0 text-lg leading-[30px]">{text}</p>
        {action}
      </div>
    </div>
  );
}
