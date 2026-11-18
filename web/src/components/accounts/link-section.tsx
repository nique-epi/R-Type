'use client';

import { useRouter } from 'next/navigation';
import { useState, useTransition } from 'react';
import { ScreenCorners } from '@/components/game/screen-corners';
import { Button } from '@/components/primitives/actions/button';
import { Notice } from '@/components/primitives/status/notice';
import { PanelTitle } from '@/components/primitives/surfaces/panel';
import { formatNodes } from '@/i18n/format-nodes';
import type { DeviceLinkDecision, DeviceLinkStatus } from '@/lib/api/schemas';
import {
  type LinkDecisionActionResult,
  type LinkDecisionOutcome,
  linkDecisionOutcomeOf,
} from '@/lib/rtype/device-link';

/** Milliseconds in a second, to read a date of the master. */
const MILLISECONDS_PER_SECOND = 1000;

/** What the page knows of the link it asks about. */
export type LinkSummary = {
  userCode: string;
  status: DeviceLinkStatus;
  /** When the game asked for it, in seconds since 1970. */
  createdAt: number;
  /** When it expires, in seconds since 1970. */
  expiresAt: number;
};

export type LinkSectionLabels = {
  title: string;
  /** "A game asked at {time} to connect to your account. Its code: {code}". */
  request: string;
  /** "The code expires at {time}.". */
  expiry: string;
  /** Says to connect only a game the visitor holds. */
  warningTitle: string;
  warning: string;
  approve: string;
  deny: string;
  pending: string;
  signOut: string;
  /** The title and the line of each end of the link. */
  outcomes: Record<
    LinkDecisionOutcome['status'] | 'expired',
    { title: string; text: string }
  >;
};

export type LinkSectionProps = {
  link: LinkSummary;
  /** Writes the decision of the visitor: the server action of the page. */
  decide: (input: {
    userCode: string;
    decision: DeviceLinkDecision;
  }) => Promise<LinkDecisionActionResult | undefined>;
  /** Signs the visitor out, to validate the game with another account; the
   * page then asks it to sign in again. */
  signOut: () => Promise<unknown>;
  /** The language the times are written in. */
  locale: string;
  labels: LinkSectionLabels;
};

/** An end the page shows instead of the two buttons. */
type LinkEnd = LinkDecisionOutcome['status'] | 'expired';

/** The end a link already stands at when the page opens, none while it waits. */
function endOf(status: DeviceLinkStatus): LinkEnd | null {
  switch (status) {
    case 'pending':
      return null;
    case 'approved':
      return 'approved';
    case 'denied':
      return 'denied';
    case 'expired':
      return 'expired';
  }
}

/** A time of the master, written in the language and the zone of the browser. */
function timeOf(seconds: number, locale: string) {
  return (
    <time
      dateTime={new Date(seconds * MILLISECONDS_PER_SECOND).toISOString()}
      suppressHydrationWarning
    >
      {new Intl.DateTimeFormat(locale, {
        hour: '2-digit',
        minute: '2-digit',
      }).format(new Date(seconds * MILLISECONDS_PER_SECOND))}
    </time>
  );
}

/**
 * What the page `/link` asks a signed-in player: whether the game that shows
 * this code, asked at this time, may sign in to its account. It says what is
 * connected and when, the guard RFC 8628 (section 5.4) gives against a code
 * someone else made a player validate; then CONNECT or REFUSE, once.
 */
export function LinkSection({
  link,
  decide,
  signOut,
  locale,
  labels,
}: LinkSectionProps) {
  const [end, setEnd] = useState<LinkEnd | null>(endOf(link.status));
  const router = useRouter();
  const [pending, startTransition] = useTransition();

  function leave() {
    startTransition(async () => {
      await signOut().catch(() => undefined);
      router.refresh();
    });
  }

  function answer(decision: DeviceLinkDecision) {
    startTransition(async () => {
      const outcome = await decide({ userCode: link.userCode, decision }).then(
        linkDecisionOutcomeOf,
        () => linkDecisionOutcomeOf(undefined),
      );
      setEnd(outcome.status);
    });
  }

  return (
    <section className="relative grid gap-4 p-6" data-slot="link-section">
      <ScreenCorners className="border-hud-cyan" />
      <PanelTitle className="m-0">{labels.title}</PanelTitle>
      <p className="m-0">
        {formatNodes(labels.request, {
          time: timeOf(link.createdAt, locale),
          code: (
            <strong className="font-normal text-star-white">
              {link.userCode}
            </strong>
          ),
        })}
      </p>
      {end ? (
        <Notice
          role="status"
          tone={end === 'approved' ? 'info' : 'warning'}
          title={labels.outcomes[end].title}
        >
          {labels.outcomes[end].text}
        </Notice>
      ) : (
        <>
          <p className="m-0">
            {formatNodes(labels.expiry, {
              time: timeOf(link.expiresAt, locale),
            })}
          </p>
          <Notice tone="warning" title={labels.warningTitle}>
            {labels.warning}
          </Notice>
          <div className="flex flex-wrap items-center gap-4">
            <Button
              variant="solid"
              disabled={pending}
              onClick={() => answer('approved')}
            >
              {pending ? labels.pending : labels.approve}
            </Button>
            <Button disabled={pending} onClick={() => answer('denied')}>
              {labels.deny}
            </Button>
          </div>
        </>
      )}
      <div>
        <Button disabled={pending} onClick={leave}>
          {labels.signOut}
        </Button>
      </div>
    </section>
  );
}
