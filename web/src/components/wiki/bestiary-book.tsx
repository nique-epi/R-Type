'use client';

import type * as React from 'react';
import { useRef, useState } from 'react';
import { Button } from '@/components/primitives/button';
import { Triangle } from '@/components/primitives/triangle';
import { useMotion } from '@/hooks/use-motion';
import { format } from '@/i18n/format';
import { gsap } from '@/lib/gsap';
import { cn } from '@/lib/utils';

/** An enemy of the bestiary: one spread of the book. */
export type BestiaryEntry = {
  id: string;
  /** Its name in pixel capitals, without accents. */
  name: string;
  /** The sprite in the photo, labelled with the name. */
  sprite: React.ReactNode;
  caption: string;
  text: string;
};

export type BestiaryBookLabels = {
  book: string;
  /** What a screen reader calls the element: "livre". */
  roleDescription: string;
  title: string;
  leftPage: string;
  rightPage: string;
  sheet: string;
  contents: string;
  previous: string;
  next: string;
  /** "Page {number} / {total}". */
  pageOf: string;
};

/**
 * The bestiary as the flight log of the client draws it: a book open on two
 * cream pages, the name of the enemy stamped on the left above its photo,
 * its sheet and the contents on the right. The pages flip in four steps
 * when the reader turns them, with the buttons or the contents.
 */
export function BestiaryBook({
  entries,
  labels,
}: {
  entries: readonly BestiaryEntry[];
  labels: BestiaryBookLabels;
}) {
  const [page, setPage] = useState(0);
  const pages = useRef<HTMLDivElement>(null);
  const entry = entries[page];

  useMotion(
    () => {
      gsap.fromTo(
        pages.current,
        { scaleX: 0.2, transformOrigin: 'left center' },
        { scaleX: 1, duration: 0.32, ease: 'steps(4)' },
      );
    },
    { dependencies: [page] },
  );

  if (entry === undefined) {
    return null;
  }
  return (
    <div data-slot="bestiary-book" className="grid gap-6">
      <section
        aria-roledescription={labels.roleDescription}
        aria-label={labels.book}
        className="relative mx-auto mb-10 w-full max-w-210 rounded-3xl bg-ink px-2.5 pt-2.5 text-ink shadow-[12px_12px_0_rgb(10_11_26/60%)]"
      >
        <div
          ref={pages}
          className="flex flex-wrap gap-1 overflow-hidden rounded-t-2xl bg-ring-front"
        >
          <section aria-label={labels.leftPage} className={PAGE}>
            <p className="m-0 font-pixel text-base uppercase leading-4">
              {labels.title}
            </p>
            <div className="h-0.5 bg-pale-blue" />
            <p className="m-0 mt-1 -rotate-3 self-start border-4 border-player-dark-pink px-3.5 py-2 font-pixel text-2xl text-player-dark-pink uppercase leading-6 outline-2 outline-player-dark-pink outline-offset-2">
              {entry.name}
            </p>
            <figure className="relative m-0 mt-4 w-45 rotate-2 self-center rounded-lg bg-star-white px-2 pt-2 shadow-[4px_4px_0_rgb(10_11_26/25%)]">
              <span
                aria-hidden="true"
                className="absolute -top-2 left-1/2 -ml-7.5 h-3.5 w-15 -rotate-4 bg-star-white/55"
              />
              <div className="grid h-31 place-items-center rounded-lg bg-sky">
                {entry.sprite}
              </div>
              <figcaption className="py-2.5 pb-3 text-center small-caps">
                {entry.caption}
              </figcaption>
            </figure>
          </section>
          <section
            aria-label={labels.rightPage}
            aria-live="polite"
            className={PAGE}
          >
            <p className="m-0 font-pixel text-base uppercase leading-4">
              {labels.sheet}
            </p>
            <div className="h-0.5 bg-pale-blue" />
            <p className="m-0 text-base leading-[26px]">{entry.text}</p>
            <p className="m-0 mt-4 small-caps text-blue">{labels.contents}</p>
            <ol className="m-0 grid list-none gap-1.5 p-0">
              {entries.map((item, index) => (
                <li key={item.id}>
                  <button
                    type="button"
                    aria-current={index === page ? 'page' : undefined}
                    onClick={() => setPage(index)}
                    className={cn(
                      'flex w-full cursor-pointer items-baseline gap-2 py-1 text-left focus-visible:outline-blue',
                      index === page ? 'text-player-dark-pink' : 'text-ink',
                    )}
                  >
                    <span className="font-pixel text-xs uppercase leading-4">
                      {item.name}
                    </span>
                    <span
                      aria-hidden="true"
                      className="flex-1 -translate-y-1 border-pale-blue border-b-2 border-dotted"
                    />
                    <span className="small-caps">{index + 1}</span>
                  </button>
                </li>
              ))}
            </ol>
          </section>
        </div>
        <div
          aria-hidden="true"
          className="h-2 bg-[repeating-linear-gradient(180deg,var(--color-ring-front)_0_2px,var(--color-paper)_2px_4px)]"
        />
        <span
          aria-hidden="true"
          className="absolute -bottom-7.5 left-[calc(50%+40px)] h-7.5 w-2.5 bg-[linear-gradient(90deg,var(--color-player-pink)_0_6px,var(--color-player-dark-pink)_6px)] [clip-path:polygon(0_0,100%_0,100%_100%,50%_76%,0_100%)]"
        />
      </section>
      <div className="flex flex-wrap items-center justify-center gap-6">
        <Button
          size="sm"
          disabled={page === 0}
          onClick={() => setPage(page - 1)}
        >
          <Triangle direction="left" />
          {labels.previous}
        </Button>
        <span className="small-caps text-star-white">
          {format(labels.pageOf, { number: page + 1, total: entries.length })}
        </span>
        <Button
          size="sm"
          disabled={page === entries.length - 1}
          onClick={() => setPage(page + 1)}
        >
          {labels.next}
          <Triangle />
        </Button>
      </div>
    </div>
  );
}

/** A cream page of the book. */
const PAGE =
  'box-border flex min-h-98 flex-[1_1_320px] flex-col gap-4 bg-paper px-7 py-6';
