import type * as React from 'react';
import { Eyebrow, Heading } from '@/components/primitives/typography';
import { cn } from '@/lib/utils';

export type SectionHeaderProps = {
  eyebrow?: React.ReactNode;
  title: React.ReactNode;
  /** The id of the heading, for the `aria-labelledby` of its section. */
  titleId?: string;
  /** A link on the right, at the height of the title: "Carte du secteur ▸". */
  action?: React.ReactNode;
  className?: string;
};

/** The head of a band of the home page: a small line, a cyan title, and a link beside them. */
export function SectionHeader({
  eyebrow,
  title,
  titleId,
  action,
  className,
}: SectionHeaderProps) {
  return (
    <div
      data-slot="section-header"
      className={cn(
        'flex flex-wrap items-end justify-between gap-4',
        className,
      )}
    >
      <div>
        {eyebrow ? <Eyebrow>{eyebrow}</Eyebrow> : null}
        <Heading id={titleId} size="title">
          {title}
        </Heading>
      </div>
      {action}
    </div>
  );
}
