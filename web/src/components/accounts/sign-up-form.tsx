'use client';

import { zodResolver } from '@hookform/resolvers/zod';
import type * as React from 'react';
import { type FieldError, useForm, useWatch } from 'react-hook-form';
import { Button } from '@/components/primitives/actions/button';
import { TextField } from '@/components/primitives/inputs/text-field';
import { Notice } from '@/components/primitives/status/notice';
import { PanelTitle } from '@/components/primitives/surfaces/panel';
import { format } from '@/i18n/format';
import {
  type Credentials,
  NICKNAME_MAXIMUM_BYTES,
  PASSWORD_MAXIMUM_BYTES,
  PASSWORD_MINIMUM_BYTES,
  type SignUpForm as SignUpFormValues,
  type SignUpProblem,
  signUpFormSchema,
  textSizeOf,
} from '@/lib/rtype/sign-up';

export type SignUpFormLabels = {
  title: string;
  nickname: string;
  nicknamePlaceholder: string;
  /** "{length} / {maximum}". */
  counter: string;
  password: string;
  /** "{minimum} caractères au moins". */
  passwordHelp: string;
  confirmation: string;
  noEmailTitle: string;
  noEmail: string;
  submit: string;
  /** The button while the master is asked. */
  pending: string;
  /** The sentence of each refusal; `{minimum}` and `{maximum}` are its bounds. */
  problems: Record<SignUpProblem, string>;
};

export type SignUpFormProps = {
  /** Called with credentials the master takes, the nickname trimmed. */
  onSubmit: (credentials: Credentials) => void;
  /** The master is being asked: the button waits. */
  pending?: boolean;
  /** Why the last try gave no account, shown over the button. */
  problem?: React.ReactNode;
  labels: SignUpFormLabels;
};

const EMPTY_FORM: SignUpFormValues = {
  nickname: '',
  password: '',
  confirmation: '',
};

/** The bounds a sentence of refusal may name. */
const BOUNDS = {
  minimum: PASSWORD_MINIMUM_BYTES,
  maximum: PASSWORD_MAXIMUM_BYTES,
};

/**
 * The form that creates an account on the master: a nickname, a password and
 * the same password again. It checks them against `signUpFormSchema`, the
 * rules of the master, so nothing the master would refuse is sent; what is
 * typed stays whatever the answer. The passwords are never shown, and the
 * browser is told which is a new password, so a password manager offers one.
 */
export function SignUpForm({
  onSubmit,
  pending = false,
  problem,
  labels,
}: SignUpFormProps) {
  const form = useForm({
    resolver: zodResolver(signUpFormSchema),
    defaultValues: EMPTY_FORM,
  });
  const nickname = useWatch({ control: form.control, name: 'nickname' });
  const { errors } = form.formState;

  function sentenceOf(error: FieldError | undefined, maximum?: number) {
    const code = error?.message as SignUpProblem | undefined;
    return code
      ? format(labels.problems[code], {
          ...BOUNDS,
          ...(maximum ? { maximum } : {}),
        })
      : undefined;
  }

  return (
    <form
      onSubmit={form.handleSubmit(({ nickname: name, password }) =>
        onSubmit({ nickname: name, password }),
      )}
      noValidate
      className="grid gap-6"
      data-slot="sign-up-form"
    >
      <PanelTitle className="m-0">{labels.title}</PanelTitle>
      <TextField
        label={labels.nickname}
        placeholder={labels.nicknamePlaceholder}
        autoComplete="username"
        autoCapitalize="none"
        spellCheck={false}
        error={sentenceOf(errors.nickname, NICKNAME_MAXIMUM_BYTES)}
        help={format(labels.counter, {
          length: textSizeOf(nickname.trim()),
          maximum: NICKNAME_MAXIMUM_BYTES,
        })}
        required
        {...form.register('nickname')}
      />
      <TextField
        label={labels.password}
        type="password"
        autoComplete="new-password"
        error={sentenceOf(errors.password)}
        help={format(labels.passwordHelp, BOUNDS)}
        required
        {...form.register('password')}
      />
      <TextField
        label={labels.confirmation}
        type="password"
        autoComplete="new-password"
        error={sentenceOf(errors.confirmation)}
        required
        {...form.register('confirmation')}
      />
      <Notice title={labels.noEmailTitle}>{labels.noEmail}</Notice>
      {problem}
      <div className="flex flex-wrap items-center gap-4">
        <Button type="submit" variant="solid" disabled={pending}>
          {pending ? labels.pending : labels.submit}
        </Button>
      </div>
    </form>
  );
}
