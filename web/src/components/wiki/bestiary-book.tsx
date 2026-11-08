'use client';

import { PageFlip } from 'page-flip';
import type * as React from 'react';
import { Fragment, useEffect, useRef, useState } from 'react';
import { Button } from '@/components/primitives/actions/button';
import { Triangle } from '@/components/primitives/icons/triangle';
import { format } from '@/i18n/format';
import { MOTION_ALLOWED } from '@/lib/gsap';
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

/** The size of a page the book is drawn from: it stretches to the width it is given, two pages side by side. */
const PAGE_WIDTH = 420;
const PAGE_HEIGHT = 460;

/** Below this width of the book, in pixels, one page shows at a time. */
const SPREAD_MINIMUM_WIDTH = 2 * 340;

/** How long a leaf takes to turn, in milliseconds; the library needs more than 0, so a reader who wants no motion gets one. */
const TURN_MILLISECONDS = 900;
const NO_MOTION_MILLISECONDS = 1;

/**
 * The bestiary as the flight log of the client draws it: a book open on two
 * newsprint pages, the name of the enemy stamped on the left above its photo,
 * its sheet and the contents on the right. The leaves turn with StPageFlip,
 * which folds the page along a line and shades the fold, when the reader
 * drags a corner, or uses the buttons or the contents.
 *
 * StPageFlip moves the pages it is given into its own elements, which React
 * would then lose track of. The pages are drawn once, hidden, by React;
 * copies of them go to the book, and a click on one of the contents is
 * heard on the book, so the copies need no handler of their own.
 */
export function BestiaryBook({
  entries,
  labels,
}: {
  entries: readonly BestiaryEntry[];
  labels: BestiaryBookLabels;
}) {
  const source = useRef<HTMLDivElement>(null);
  const host = useRef<HTMLDivElement>(null);
  const book = useRef<PageFlip | null>(null);
  const [pageIndex, setPageIndex] = useState(0);
  const [portrait, setPortrait] = useState(false);
  const entryIndex = Math.floor(pageIndex / 2);

  useEffect(() => {
    const parent = host.current;
    const pages = source.current?.children;
    if (!parent || !pages) {
      return;
    }
    const root = document.createElement('div');
    parent.append(root);
    const copies = [...pages].map(page => page.cloneNode(true) as HTMLElement);
    root.append(...copies);
    const flip = new PageFlip(root, {
      width: PAGE_WIDTH,
      height: PAGE_HEIGHT,
      size: 'stretch',
      minWidth: SPREAD_MINIMUM_WIDTH / 2,
      maxWidth: PAGE_WIDTH,
      maxShadowOpacity: 0.6,
      showCover: false,
      mobileScrollSupport: false,
      disableFlipByClick: true,
      showPageCorners: true,
      flippingTime: window.matchMedia(MOTION_ALLOWED).matches
        ? TURN_MILLISECONDS
        : NO_MOTION_MILLISECONDS,
    });
    flip.loadFromHTML(copies);
    flip.on('flip', event => setPageIndex(Number(event.data)));
    flip.on('init', () => setPortrait(flip.getOrientation() === 'portrait'));
    flip.on('changeOrientation', event => {
      setPortrait(event.data === 'portrait');
      requestAnimationFrame(() => flip.update());
    });
    // A page is drawn at its own size and scaled to the width the book gives it.
    const scaling = new ResizeObserver(() => {
      for (const page of copies) {
        // The library rewrites the style of the page, so the scale goes on its content.
        const content = page.firstElementChild;
        if (content instanceof HTMLElement && page.clientWidth > 0) {
          content.style.setProperty(
            '--page-scale',
            String(page.clientWidth / PAGE_WIDTH),
          );
        }
      }
    });
    copies.forEach(page => void scaling.observe(page));
    // The library measures its container when it starts and when the window resizes; the container may settle later.
    const resizing = new ResizeObserver(() => flip.update());
    resizing.observe(parent);
    function turnToContents(event: MouseEvent) {
      const entry = (event.target as Element).closest<HTMLElement>(
        '[data-turn-to]',
      );
      if (entry) {
        flip.flip(Number(entry.dataset.turnTo) * 2);
      }
    }
    root.addEventListener('click', turnToContents);
    book.current = flip;
    return () => {
      root.removeEventListener('click', turnToContents);
      scaling.disconnect();
      resizing.disconnect();
      flip.destroy();
      book.current = null;
    };
  }, []);

  const last = entries.length - 1;
  return (
    <div data-slot="bestiary-book" className="grid gap-6">
      <section
        aria-roledescription={labels.roleDescription}
        aria-label={labels.book}
        className="relative mx-auto mb-10 w-full max-w-210 rounded-3xl bg-ink px-2.5 pt-2.5 text-ink shadow-[12px_12px_0_rgb(10_11_26/60%)]"
      >
        <div
          ref={host}
          className={cn(
            'mx-auto w-full overflow-hidden rounded-t-2xl bg-ring-front',
            portrait ? 'h-[460px]' : 'aspect-[840/460]',
          )}
        />
        <div ref={source} hidden>
          {entries.map(entry => (
            <Fragment key={entry.id}>
              <div className={cn('page', PAGE, LEFT_PAGE)}>
                <div className={PAGE_CONTENT}>
                  <LeftPageContent entry={entry} labels={labels} />
                </div>
              </div>
              <div className={cn('page', PAGE, RIGHT_PAGE)}>
                <div className={PAGE_CONTENT}>
                  <RightPageContent
                    entry={entry}
                    entries={entries}
                    current={entries.indexOf(entry)}
                    labels={labels}
                  />
                </div>
              </div>
            </Fragment>
          ))}
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
          disabled={entryIndex === 0}
          onClick={() => book.current?.flipPrev()}
        >
          <Triangle direction="left" />
          {labels.previous}
        </Button>
        <span className="small-caps text-star-white">
          {format(labels.pageOf, {
            number: entryIndex + 1,
            total: entries.length,
          })}
        </span>
        <Button
          size="sm"
          disabled={entryIndex === last}
          onClick={() => book.current?.flipNext()}
        >
          {labels.next}
          <Triangle />
        </Button>
      </div>
    </div>
  );
}

/** The left page: the name stamped above the photo of the enemy. */
function LeftPageContent({
  entry,
  labels,
}: {
  entry: BestiaryEntry;
  labels: BestiaryBookLabels;
}) {
  return (
    <>
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
    </>
  );
}

/** The right page: the sheet of the enemy and the contents, each line of which turns to its page. */
function RightPageContent({
  entry,
  entries,
  current,
  labels,
}: {
  entry: BestiaryEntry;
  entries: readonly BestiaryEntry[];
  current: number;
  labels: BestiaryBookLabels;
}) {
  return (
    <>
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
              data-turn-to={index}
              aria-current={index === current ? 'page' : undefined}
              className={cn(
                'flex w-full cursor-pointer items-baseline gap-2 py-1 text-left focus-visible:outline-blue',
                index === current ? 'text-player-dark-pink' : 'text-ink',
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
    </>
  );
}

/** A page of the book: newsprint, aged at its edges. StPageFlip sets its display, so what lays out its content is one element inside. */
const PAGE = 'newsprint overflow-hidden';

/** What lays out the content of a page. */
const PAGE_CONTENT =
  'box-border flex h-[460px] w-[420px] origin-top-left flex-col gap-4 px-7 py-6 [transform:scale(var(--page-scale,1))]';

/** The fold of the spine darkens the inner edge of each page. */
const LEFT_PAGE =
  'shadow-[inset_-22px_0_22px_-14px_rgb(80_55_20/30%),inset_0_0_38px_rgb(150_110_50/22%)]';
const RIGHT_PAGE =
  'shadow-[inset_22px_0_22px_-14px_rgb(80_55_20/30%),inset_0_0_38px_rgb(150_110_50/22%)]';
