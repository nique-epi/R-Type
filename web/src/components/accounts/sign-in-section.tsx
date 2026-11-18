'use client';

import { zodResolver } from '@hookform/resolvers/zod';
import { useRouter } from 'next/navigation';
import { useState, useTransition } from 'react';
import { type FieldError, useForm } from 'react-hook-form';
import { Button } from '@/components/primitives/actions/button';
import { TextField } from '@/components/primitives/inputs/text-field';
import { Notice } from '@/components/primitives/status/notice';
import { PanelTitle } from '@/components/primitives/surfaces/panel';
import {
  type SignIn,
  type SignInForm,
  type SignInOutcome,
  type SignInProblem,
  signInFormSchema,
  signInOutcomeOf,
} from '@/lib/rtype/sign-in';

/** An outcome with no session: why the master opened none. */
type SignInRefusal = Exclude<SignInOutcome['status'], 'signedIn'>;

export type SignInSectionLabels = {
  form: {
    title: string;
    nickname: string;
    password: string;
    submit: string;
    /** The button while the master is asked. */
    pending: string;
    problems: Record<SignInProblem, string>;
  };
  /** The title and the line of each outcome with no session. */
  problems: Record<SignInRefusal, { title: string; text: string }>;
  /** Shown while the page leads to the next one. */
  signedIn: string;
};

export type SignInSectionProps = {
  /** Opens a session on the master: the server action of the page. */
  signIn: SignIn;
  /** The page of the site to go to once signed in. */
  nextHref: string;
  labels: SignInSectionLabels;
};

const EMPTY_FORM: SignInForm = { nickname: '', password: '' };

/**
 * The sign-in form: a nickname and a password, which the browser may fill
 * from its password manager. A try that opened no session says why over the
 * form; once signed in, the page leads to `nextHref`, such as the page that
 * validates a game.
 */
export function SignInSection({
  signIn,
  nextHref,
  labels,
}: SignInSectionProps) {
  const router = useRouter();
  const [problem, setProblem] = useState<SignInRefusal | null>(null);
  const [signedIn, setSignedIn] = useState(false);
  const [pending, startTransition] = useTransition();
  const form = useForm({
    resolver: zodResolver(signInFormSchema),
    defaultValues: EMPTY_FORM,
  });
  const { errors } = form.formState;

  function sentenceOf(error: FieldError | undefined) {
    const code = error?.message as SignInProblem | undefined;
    return code ? labels.form.problems[code] : undefined;
  }

  function submit(values: SignInForm) {
    startTransition(async () => {
      const outcome = await signIn(values).then(signInOutcomeOf, () =>
        signInOutcomeOf(undefined),
      );
      if (outcome.status === 'signedIn') {
        setSignedIn(true);
        router.replace(nextHref);
        router.refresh();
        return;
      }
      setProblem(outcome.status);
    });
  }

  if (signedIn) {
    return <Notice title={labels.form.title}>{labels.signedIn}</Notice>;
  }

  return (
    <form
      onSubmit={form.handleSubmit(submit)}
      noValidate
      className="grid gap-6"
      data-slot="sign-in-form"
    >
      <PanelTitle className="m-0">{labels.form.title}</PanelTitle>
      <TextField
        label={labels.form.nickname}
        autoComplete="username"
        autoCapitalize="none"
        spellCheck={false}
        error={sentenceOf(errors.nickname)}
        required
        {...form.register('nickname')}
      />
      <TextField
        label={labels.form.password}
        type="password"
        autoComplete="current-password"
        error={sentenceOf(errors.password)}
        required
        {...form.register('password')}
      />
      {problem ? (
        <Notice
          role="alert"
          tone="warning"
          title={labels.problems[problem].title}
        >
          {labels.problems[problem].text}
        </Notice>
      ) : null}
      <div className="flex flex-wrap items-center gap-4">
        <Button type="submit" variant="solid" disabled={pending}>
          {pending ? labels.form.pending : labels.form.submit}
        </Button>
      </div>
    </form>
  );
}
