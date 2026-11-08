'use client';

import Link from 'next/link';
import { usePathname } from 'next/navigation';
import type * as React from 'react';
import { useRef } from 'react';
import { Button } from '@/components/primitives/actions/button';
import { IconButton } from '@/components/primitives/actions/icon-button';
import { PixelIcon } from '@/components/primitives/icons/pixel-icon';
import { Dialog } from '@/components/primitives/surfaces/dialog';
import { StarLayers } from '@/components/sky/star-layers';
import { Logo } from '@/components/sprites/logo';
import { Ship } from '@/components/sprites/ship';
import { motionAllowed, useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';
import { cn } from '@/lib/utils';
import { isCurrent, type NavItem } from './nav-item';

export type SiteMenuLabels = {
  menu: string;
  close: string;
  navigation: string;
  logo: string;
  download: string;
};

export type SiteMenuProps = {
  open: boolean;
  onClose: () => void;
  items: readonly NavItem[];
  downloadHref: string;
  /** The line under the download button: "v0.1.0 · Linux · Windows · macOS". */
  footnote: React.ReactNode;
  labels: SiteMenuLabels;
};

/**
 * The menu of every page: a panel that slides in from the right over a
 * dimmed page, its links large, the current page marked. It is a modal
 * dialog: the focus stays in it, Escape and a click beside it close it.
 */
export function SiteMenu({
  open,
  onClose,
  items,
  downloadHref,
  footnote,
  labels,
}: SiteMenuProps) {
  const panel = useRef<HTMLDialogElement>(null);
  const pathname = usePathname();

  const { contextSafe } = useMotion(
    () => {
      if (!open) {
        return;
      }
      gsap.fromTo(
        panel.current,
        { xPercent: 110 },
        { xPercent: 0, duration: 0.65, ease: 'power4.out' },
      );
      gsap.fromTo(
        '[data-menu-item]',
        { x: 48, opacity: 0 },
        {
          x: 0,
          opacity: 1,
          duration: 0.6,
          ease: 'power3.out',
          stagger: 0.05,
          delay: 0.1,
        },
      );
    },
    { scope: panel, dependencies: [open] },
  );

  const close = contextSafe(() => {
    if (!motionAllowed()) {
      onClose();
      return;
    }
    gsap
      .timeline({ onComplete: onClose })
      .to(
        '[data-menu-item]',
        { x: 32, opacity: 0, duration: 0.2, ease: 'power2.in', stagger: 0.02 },
        0,
      )
      .to(
        panel.current,
        { xPercent: 110, duration: 0.4, ease: 'power3.in' },
        0,
      );
  });

  return (
    <Dialog
      ref={panel}
      open={open}
      onClose={close}
      aria-label={labels.menu}
      data-lenis-prevent
      className="inset-y-3 right-3 left-auto h-auto w-[min(440px,calc(100vw-24px))] overflow-auto rounded-[32px] bg-sky shadow-[0_24px_64px_rgb(10_11_26/70%),inset_0_0_0_2px_rgb(38_48_94/80%)] backdrop:bg-sky/60 backdrop:backdrop-blur-sm"
    >
      <div className="relative flex min-h-full flex-col px-8 pt-6 pb-8 max-md:px-6 max-md:pt-5 max-md:pb-6">
        {open ? (
          <StarLayers layers={['far', 'mid']} className="opacity-70" />
        ) : null}
        <div
          data-menu-item
          className="relative mb-10 flex items-center justify-between"
        >
          <Logo alt={labels.logo} />
          <IconButton aria-label={labels.close} onClick={close}>
            <PixelIcon name="close" />
          </IconButton>
        </div>
        <nav aria-label={labels.navigation} className="relative">
          <ul className="m-0 grid list-none p-0">
            {items.map(item => {
              const current = isCurrent(item.href, pathname, item.exact);
              return (
                <li key={item.href} data-menu-item>
                  <Link
                    href={item.href}
                    onClick={close}
                    aria-current={current ? 'page' : undefined}
                    className={cn(
                      'row-link group/item relative -mx-4 flex items-center gap-3.5 px-4 py-3.5 font-pixel text-xl uppercase leading-7 transition-colors duration-200 hover:bg-deep-blue/55 hover:text-star-white focus-visible:bg-deep-blue/55 focus-visible:text-star-white max-md:text-base max-md:leading-6',
                      current ? 'text-hud-cyan' : 'text-pale-blue',
                    )}
                  >
                    <span
                      aria-hidden="true"
                      className={cn(
                        'size-2.5 rounded-full bg-current transition-transform duration-200 group-hover/item:scale-100',
                        current ? 'scale-100' : 'scale-0',
                      )}
                    />
                    {item.label}
                  </Link>
                </li>
              );
            })}
          </ul>
        </nav>
        <div className="relative mt-auto grid gap-5 pt-8">
          <Ship data-menu-item player={1} />
          <Button
            data-menu-item
            href={downloadHref}
            onClick={close}
            variant="solid"
            className="justify-self-start"
          >
            {labels.download}
          </Button>
          <p data-menu-item className="m-0 small-caps text-pale-blue">
            {footnote}
          </p>
        </div>
      </div>
    </Dialog>
  );
}
