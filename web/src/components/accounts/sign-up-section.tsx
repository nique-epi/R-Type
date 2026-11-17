'use client';

import { useId, useState, useTransition } from 'react';
import { ScreenCorners } from '@/components/game/screen-corners';
import { Button } from '@/components/primitives/actions/button';
import { Notice } from '@/components/primitives/status/notice';
import { PanelTitle } from '@/components/primitives/surfaces/panel';
import { formatNodes } from '@/i18n/format-nodes';
import {
  type CreatedUser,
  type Credentials,
  outcomeOf,
  type SignUp,
  type SignUpOutcome,
} from '@/lib/rtype/sign-up';
import { SignUpForm, type SignUpFormLabels } from './sign-up-form';

/** An outcome with no account: why the master gave none. */
type SignUpRefusal = Exclude<SignUpOutcome['status'], 'created'>;

export type SignUpSectionLabels = {
  form: SignUpFormLabels;
  /** The title and the line of each outcome with no account. */
  problems: Record<SignUpRefusal, { title: string; text: string }>;
  /** "Bienvenue, {nickname}. Votre pseudo est réservé.". */
  created: { title: string; welcome: string };
  home: string;
  download: string;
};

export type SignUpSectionProps = {
  /** Asks the master for an account: the server action of the page. */
  signUp: SignUp;
  homeHref: string;
  downloadHref: string;
  labels: SignUpSectionLabels;
};

/**
 * The sign-up page under its hero: the form, then the account the master
 * created. A try that gave no account says why over the form, which keeps
 * what was typed; once the account exists the form is gone, and the
 * passwords with it.
 */
export function SignUpSection({
  signUp,
  homeHref,
  downloadHref,
  labels,
}: SignUpSectionProps) {
  const titleId = useId();
  const [created, setCreated] = useState<CreatedUser | null>(null);
  const [problem, setProblem] = useState<SignUpRefusal | null>(null);
  const [pending, startTransition] = useTransition();

  function create(credentials: Credentials) {
    startTransition(async () => {
      const outcome = await signUp(credentials).then(outcomeOf, () =>
        outcomeOf(undefined),
      );
      if (outcome.status === 'created') {
        setCreated(outcome.user);
        setProblem(null);
      } else {
        setProblem(outcome.status);
      }
    });
  }

  if (created) {
    return (
      <section
        aria-labelledby={titleId}
        className="relative grid gap-4 p-6"
        data-slot="sign-up-created"
      >
        <ScreenCorners className="border-hud-cyan" />
        <PanelTitle id={titleId} className="m-0">
          {labels.created.title}
        </PanelTitle>
        <p className="m-0">
          {formatNodes(labels.created.welcome, {
            nickname: (
              <strong className="font-normal text-star-white">
                {created.nickname}
              </strong>
            ),
          })}
        </p>
        <div className="flex flex-wrap items-center gap-4">
          <Button variant="solid" href={homeHref}>
            {labels.home}
          </Button>
          <Button href={downloadHref}>{labels.download}</Button>
        </div>
      </section>
    );
  }

  return (
    <SignUpForm
      onSubmit={create}
      pending={pending}
      labels={labels.form}
      problem={
        problem ? (
          <Notice
            role="alert"
            tone="warning"
            title={labels.problems[problem].title}
          >
            {labels.problems[problem].text}
          </Notice>
        ) : null
      }
    />
  );
}
