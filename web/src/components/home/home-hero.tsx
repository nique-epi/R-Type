'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { Heading } from '@/components/primitives/text/typography';
import { Planet } from '@/components/sky/planet';
import { StarLayers } from '@/components/sky/star-layers';
import { Logo } from '@/components/sprites/logo';
import { Ship } from '@/components/sprites/ship';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';
import { PLAYER_NUMBERS } from '@/lib/rtype/players';

/** Where each ship of the squadron flies, in fractions of the window: its top, its first and last x. */
const SQUADRON = [
  { top: '16%', from: 0.16, to: 0.82 },
  { top: '36%', from: 0.05, to: 0.64 },
  { top: '60%', from: 0.2, to: 0.9 },
  { top: '76%', from: 0.09, to: 0.72 },
] as const;

/** Pixels per second of the planet across the sky, as in a game. */
const PLANET_SPEED = 10;

/** How far a shot flies before it fades, in pixels. */
const SHOT_RANGE = 900;

export type HomeHeroProps = {
  /** "Disponible maintenant · v0.1.0". */
  eyebrow: React.ReactNode;
  logoAlt: string;
  /** The buttons under the logo: JOUER, TELECHARGER. */
  actions: React.ReactNode;
  /** The line that rises as the hero scrolls: "Quatre pilotes. Une vague de Bydos." */
  tagline: React.ReactNode;
  /** The facts at the foot of the hero: players, systems. */
  facts: React.ReactNode;
  'aria-label': string;
};

/**
 * The top of the home page, a scene of the game two windows tall: the stage
 * sticks while the page scrolls past it, the squadron hovers and fires, the
 * planet crosses the sky, and the scroll carries the ships across the screen
 * while the logo shrinks away and the tagline rises.
 */
export function HomeHero({
  eyebrow,
  logoAlt,
  actions,
  tagline,
  facts,
  'aria-label': label,
}: HomeHeroProps) {
  const hero = useRef<HTMLElement>(null);

  useMotion(
    () => {
      const width = () => window.innerWidth;
      gsap.from('[data-logo-part]', {
        y: 32,
        opacity: 0,
        duration: 1,
        ease: 'power3.out',
        stagger: 0.12,
      });
      gsap.from('[data-facts] > *', {
        y: 12,
        opacity: 0,
        duration: 0.8,
        ease: 'power2.out',
        stagger: 0.06,
        delay: 0.6,
      });

      const planet =
        hero.current?.querySelector<HTMLElement>('[data-slot=planet]');
      if (planet) {
        const start = planet.offsetLeft;
        const span = planet.offsetWidth + 40;
        gsap.from(planet, { opacity: 0, duration: 1.6, ease: 'power2.out' });
        gsap.to(planet, {
          x: -(start + span),
          duration: (start + span) / PLANET_SPEED,
          ease: 'none',
          onComplete: () => {
            gsap.fromTo(
              planet,
              { x: width() - start + 40 },
              {
                x: -(start + span),
                duration: (width() + span + 40) / PLANET_SPEED,
                ease: 'none',
                repeat: -1,
              },
            );
          },
        });
      }

      const flyers = gsap.utils.toArray<HTMLElement>('[data-flyer]');
      flyers.forEach((flyer, index) => {
        gsap.set(flyer, { x: width() * (SQUADRON[index]?.from ?? 0) });
        gsap.to(flyer, {
          y: 'random(-12, 12, 2)',
          duration: 'random(1.4, 2.2)',
          ease: 'sine.inOut',
          repeat: -1,
          yoyo: true,
          repeatRefresh: true,
        });
        gsap
          .timeline({
            repeat: -1,
            repeatDelay: 0.9 + index * 0.35,
            delay: index * 0.4,
          })
          .fromTo(
            flyer.querySelector('[data-shot]'),
            { x: 0, opacity: 1 },
            { x: SHOT_RANGE, opacity: 0, duration: 0.8, ease: 'none' },
          );
      });

      gsap
        .timeline({
          scrollTrigger: {
            trigger: hero.current,
            start: 'top top',
            end: 'bottom bottom',
            scrub: 0.8,
          },
        })
        .to(
          '[data-logo-block]',
          {
            scale: 0.6,
            yPercent: -40,
            opacity: 0,
            ease: 'none',
            duration: 0.5,
          },
          0,
        )
        .to(
          planet ?? [],
          { scale: 1.25, yPercent: -12, ease: 'none', duration: 1 },
          0,
        )
        .to('[data-layer=near]', { yPercent: -6, ease: 'none', duration: 1 }, 0)
        .fromTo(
          flyers,
          { x: index => width() * (SQUADRON[index]?.from ?? 0) },
          {
            x: index => width() * (SQUADRON[index]?.to ?? 0),
            ease: 'none',
            duration: 1,
          },
          0,
        )
        .to('[data-facts]', { opacity: 0, ease: 'none', duration: 0.2 }, 0)
        .fromTo(
          '[data-tagline]',
          { opacity: 0, y: 48 },
          { opacity: 1, y: 0, ease: 'none', duration: 0.4 },
          0.45,
        );
    },
    { scope: hero },
  );

  return (
    <section
      ref={hero}
      aria-label={label}
      data-slot="home-hero"
      className="relative -mt-[76px] h-[220vh] max-md:h-[170vh]"
    >
      <div className="sticky top-0 h-screen min-h-[620px] overflow-hidden">
        <StarLayers layers={['far', 'mid']} />
        <Planet size="large" />
        <StarLayers layers={['near']} />
        <div
          aria-hidden="true"
          className="pointer-events-none absolute inset-0 overflow-hidden"
        >
          {PLAYER_NUMBERS.map((player, index) => (
            <span
              key={player}
              data-flyer
              className="absolute left-0 block"
              style={{ top: SQUADRON[index]?.top }}
            >
              <Ship player={player} scale={4} narrowScale={2} />
              <span
                data-shot
                className="absolute top-7 left-30 h-2 w-10 rounded-sm bg-linear-to-r from-hud-cyan/0 via-hud-cyan via-60% to-star-white opacity-0 max-md:top-3 max-md:left-15 max-md:h-1.5 max-md:w-4"
              />
            </span>
          ))}
        </div>
        <div className="absolute inset-0 flex flex-col items-center justify-center gap-6 px-4 text-center">
          <div data-logo-block className="flex flex-col items-center gap-5">
            <p data-logo-part className="m-0 small-caps text-star-white">
              {eyebrow}
            </p>
            <h1 data-logo-part className="m-0">
              <Logo
                scale={3}
                alt={logoAlt}
                priority
                className="w-[630px] max-w-[84vw]"
              />
            </h1>
            <div
              data-logo-part
              className="mt-3 flex flex-wrap justify-center gap-4"
            >
              {actions}
            </div>
          </div>
        </div>
        <Heading
          as="p"
          size="display"
          data-tagline
          className="absolute inset-x-0 bottom-[16vh] px-4 text-center opacity-0"
        >
          {tagline}
        </Heading>
        <div
          data-facts
          className="absolute inset-x-0 bottom-8 flex flex-wrap justify-center gap-x-8 gap-y-3 small-caps text-pale-blue [&_b]:font-normal [&_b]:text-star-white"
        >
          {facts}
        </div>
      </div>
    </section>
  );
}
