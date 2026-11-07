import type * as React from 'react';
import { Container } from '@/components/primitives/container';
import { Heading, Lede } from '@/components/primitives/typography';
import { StarLayers } from '@/components/sky/star-layers';
import { cn } from '@/lib/utils';

export type StoryBandProps = {
  /** A figure over the title, such as the professor. */
  figure?: React.ReactNode;
  title: React.ReactNode;
  titleId?: string;
  lede?: React.ReactNode;
  /** Buttons or links under the lede. */
  children?: React.ReactNode;
  align?: 'center' | 'start';
  className?: string;
};

/**
 * A band of sky with rounded corners across the page, for a line that
 * stands alone: "Chaque pilote prêt lance la partie.", "Le code est ouvert".
 */
export function StoryBand({
  figure,
  title,
  titleId,
  lede,
  children,
  align = 'center',
  className,
}: StoryBandProps) {
  const centered = align === 'center';
  return (
    <section
      aria-labelledby={titleId}
      data-slot="story-band"
      className={cn(
        'relative mx-4 overflow-hidden rounded-[40px] py-30 max-md:py-18',
        centered && 'text-center',
        className,
      )}
    >
      <StarLayers layers={centered ? ['far', 'mid'] : ['far']} />
      <Container
        className={cn(
          'relative grid gap-6',
          centered && 'justify-items-center',
        )}
      >
        {figure ? <div className="mb-2">{figure}</div> : null}
        <Heading id={titleId} size={centered ? 'display' : 'title'}>
          {title}
        </Heading>
        {lede ? (
          <Lede className={cn('mt-0', centered && 'mx-auto')}>{lede}</Lede>
        ) : null}
        {children ? (
          <div className="flex flex-wrap gap-4">{children}</div>
        ) : null}
      </Container>
    </section>
  );
}
