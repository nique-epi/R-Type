'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { TypeIn } from '@/components/motion/type-in';
import { Container } from '@/components/primitives/layout/container';
import {
  Eyebrow,
  Heading,
  Lede,
} from '@/components/primitives/text/typography';
import { Planet } from '@/components/sky/planet';
import { StarLayers } from '@/components/sky/star-layers';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';

export type PageHeroProps = {
  /** The line over the title: a breadcrumb, a date, a section. */
  eyebrow?: React.ReactNode;
  title: React.ReactNode;
  lede?: React.ReactNode;
  /** What goes under the lede, such as the status of a server. */
  children?: React.ReactNode;
};

/**
 * The top of an inner page: the sky behind the header, a planet in the
 * corner, the title written in like a line of the HUD. As the page scrolls,
 * the title rises and fades while the sky and the planet drift at their own
 * depth.
 */
export function PageHero({ eyebrow, title, lede, children }: PageHeroProps) {
  const hero = useRef<HTMLElement>(null);

  useMotion(
    () => {
      gsap.from('[data-hero-in]', {
        y: 32,
        opacity: 0,
        duration: 0.9,
        ease: 'power3.out',
        stagger: 0.1,
      });
      gsap.to('[data-slot=planet]', {
        x: -120,
        duration: 24,
        ease: 'sine.inOut',
        repeat: -1,
        yoyo: true,
      });
      gsap
        .timeline({
          scrollTrigger: {
            trigger: hero.current,
            start: 'top top',
            end: 'bottom top',
            scrub: 0.6,
          },
        })
        .to('[data-hero-text]', { yPercent: 35, opacity: 0, ease: 'none' }, 0)
        .to(
          '[data-hero-depth=planet]',
          { yPercent: 45, scale: 1.25, ease: 'none' },
          0,
        )
        .to('[data-layer=mid]', { yPercent: -12, ease: 'none' }, 0)
        .to('[data-layer=far]', { yPercent: -4, ease: 'none' }, 0);
    },
    { scope: hero },
  );

  return (
    <section
      ref={hero}
      data-slot="page-hero"
      className="relative -mt-[76px] overflow-hidden rounded-b-[40px] bg-sky pt-41 pb-16 max-md:pt-30"
    >
      <StarLayers layers={['far', 'mid']} />
      <div data-hero-depth="planet" className="absolute inset-0">
        <Planet size="small" />
      </div>
      <Container data-hero-text className="relative">
        {eyebrow ? <Eyebrow data-hero-in>{eyebrow}</Eyebrow> : null}
        <TypeIn>
          <Heading as="h1" size="display" data-hero-in>
            {title}
          </Heading>
        </TypeIn>
        {lede ? (
          <Lede data-hero-in className="text-pale-blue">
            {lede}
          </Lede>
        ) : null}
        {children ? (
          <div data-hero-in className="mt-6">
            {children}
          </div>
        ) : null}
      </Container>
    </section>
  );
}
