'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { Float } from '@/components/motion/float';
import {
  Eyebrow,
  Heading,
  Lede,
} from '@/components/primitives/text/typography';
import { useMotion } from '@/hooks/use-motion';
import { WIDE_SCREEN_QUERY } from '@/lib/breakpoints';
import { gsap } from '@/lib/gsap';

export type BydoPanel = {
  id: string;
  /** "01 / 05". */
  number: string;
  name: string;
  text: string;
  /** The sprite of the enemy, labelled with its name. */
  sprite: React.ReactNode;
};

export type BydoParadeProps = {
  eyebrow: React.ReactNode;
  title: React.ReactNode;
  lede: React.ReactNode;
  /** A link to the bestiary of the wiki, under the lede. */
  action: React.ReactNode;
  panels: readonly BydoPanel[];
  'aria-label': string;
};

/**
 * The enemies of the game, one panel each: on a wide screen the section is
 * as tall as the panels are wide, its frame sticks, and the scroll slides
 * the panels sideways; on a phone they stack.
 */
export function BydoParade({
  eyebrow,
  title,
  lede,
  action,
  panels,
  'aria-label': label,
}: BydoParadeProps) {
  const section = useRef<HTMLElement>(null);
  const track = useRef<HTMLDivElement>(null);

  useMotion(
    () => {
      gsap.to(track.current, {
        x: () =>
          -Math.max(0, (track.current?.scrollWidth ?? 0) - window.innerWidth),
        ease: 'none',
        scrollTrigger: {
          trigger: section.current,
          start: 'top top',
          end: 'bottom bottom',
          scrub: 0.8,
          invalidateOnRefresh: true,
        },
      });
    },
    { scope: section, query: WIDE_SCREEN_QUERY },
  );

  return (
    <section
      ref={section}
      aria-label={label}
      data-slot="bydo-parade"
      className="relative h-[calc(100vh+360vw)] max-md:h-auto"
    >
      <div className="sticky top-0 h-screen min-h-[620px] overflow-hidden max-md:static max-md:h-auto max-md:min-h-0">
        <div
          ref={track}
          className="flex h-full w-max max-md:w-auto max-md:flex-col"
        >
          <div className="box-border flex h-full w-[60vw] flex-none flex-col justify-center px-[8vw] max-md:h-auto max-md:w-auto max-md:px-4 max-md:py-12">
            <Eyebrow>{eyebrow}</Eyebrow>
            <Heading size="display">{title}</Heading>
            <Lede>{lede}</Lede>
            <div className="mt-8">{action}</div>
          </div>
          {panels.map(panel => (
            <article
              key={panel.id}
              className="box-border flex h-full w-[80vw] flex-none items-center justify-center gap-[5vw] px-[6vw] max-md:h-auto max-md:w-auto max-md:flex-col max-md:gap-6 max-md:px-4 max-md:py-12"
            >
              <Float>{panel.sprite}</Float>
              <div className="grid max-w-[440px] gap-5">
                <span className="small-caps text-star-white">
                  {panel.number}
                </span>
                <Heading as="h3" size="giant">
                  {panel.name}
                </Heading>
                <p className="m-0 text-lg leading-[30px]">{panel.text}</p>
              </div>
            </article>
          ))}
        </div>
      </div>
    </section>
  );
}
