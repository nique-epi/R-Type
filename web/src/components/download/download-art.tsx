'use client';

import { useRef } from 'react';
import { Enemy } from '@/components/sprites/enemy';
import { Ship } from '@/components/sprites/ship';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';

/**
 * The scene beside the download button: two ships that drift in steps and a
 * Voltjelly that bobs, the whole scene rising a little as the page scrolls.
 * Hidden on a phone, where the button needs the room.
 */
export function DownloadArt() {
  const scene = useRef<HTMLDivElement>(null);

  useMotion(
    () => {
      gsap.utils
        .toArray<HTMLElement>('[data-drifter]')
        .forEach((ship, index) => {
          gsap.to(ship, {
            x: 'random(0, 24, 2)',
            y: 'random(-10, 10, 2)',
            duration: 'random(1.2, 1.8)',
            ease: 'steps(8)',
            repeat: -1,
            yoyo: true,
            repeatRefresh: true,
            delay: index * 0.3,
          });
        });
      gsap.to('[data-foe]', {
        y: -12,
        duration: 1,
        ease: 'steps(6)',
        repeat: -1,
        yoyo: true,
      });
      gsap.to(scene.current, {
        yPercent: -18,
        ease: 'none',
        scrollTrigger: {
          trigger: scene.current,
          start: 'top bottom',
          end: 'bottom top',
          scrub: true,
        },
      });
    },
    { scope: scene },
  );

  return (
    <div
      ref={scene}
      aria-hidden="true"
      data-slot="download-art"
      className="relative h-[220px] flex-[1_1_240px] max-md:hidden"
    >
      <span data-drifter className="absolute top-10 left-5">
        <Ship player={1} scale={4} />
      </span>
      <span data-drifter className="absolute top-35 left-[70px]">
        <Ship player={3} />
      </span>
      <span data-foe className="absolute top-[70px] right-2.5">
        <Enemy kind="voltjelly" scale={2} />
      </span>
    </div>
  );
}
