'use client';

import type * as React from 'react';
import { useState } from 'react';
import { Reveal } from '@/components/motion/reveal';
import { Button } from '@/components/primitives/button';
import { Notice } from '@/components/primitives/notice';
import { PanelTitle } from '@/components/primitives/panel';
import { TextField } from '@/components/primitives/text-field';
import { format } from '@/i18n/format';

/** The longest name of a server the master takes. */
export const SERVER_NAME_MAXIMUM = 32;

export type HostServerFormLabels = {
  title: string;
  name: string;
  placeholder: string;
  /** "{length} / {maximum}". */
  counter: string;
  nameRequired: string;
  withoutAccountTitle: string;
  withoutAccount: string;
  quotaTitle: string;
  submit: string;
  cancel: string;
};

export type HostServerFormProps = {
  onSubmit: (name: string) => void;
  onCancel: () => void;
  /** Set when the master refused for the quota: "Réessayez dans 38 min." */
  quotaMessage?: React.ReactNode;
  pending?: boolean;
  labels: HostServerFormLabels;
};

/**
 * The form that creates a game server on the master: a name of at most 32
 * characters, the rules of hosting without an account, the quota when it is
 * reached. An empty name is refused before anything is sent.
 */
export function HostServerForm({
  onSubmit,
  onCancel,
  quotaMessage,
  pending = false,
  labels,
}: HostServerFormProps) {
  const [name, setName] = useState('');
  const [tried, setTried] = useState(false);
  const trimmed = name.trim();
  const refused = tried && trimmed === '';

  function submit(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();
    setTried(true);
    if (trimmed !== '') {
      onSubmit(trimmed);
    }
  }

  return (
    <form onSubmit={submit} noValidate data-slot="host-server-form">
      <Reveal from={{ y: 24 }} stagger={0.07} className="grid gap-6">
        <PanelTitle className="m-0">{labels.title}</PanelTitle>
        <TextField
          label={labels.name}
          value={name}
          onChange={event => setName(event.target.value)}
          maxLength={SERVER_NAME_MAXIMUM}
          placeholder={labels.placeholder}
          error={refused ? labels.nameRequired : undefined}
          help={format(labels.counter, {
            length: name.length,
            maximum: SERVER_NAME_MAXIMUM,
          })}
          required
        />
        <Notice title={labels.withoutAccountTitle}>
          {labels.withoutAccount}
        </Notice>
        {quotaMessage ? (
          <Notice tone="warning" title={labels.quotaTitle}>
            {quotaMessage}
          </Notice>
        ) : null}
        <div className="flex flex-wrap items-center gap-4">
          <Button
            type="submit"
            variant="solid"
            disabled={Boolean(quotaMessage) || pending}
          >
            {labels.submit}
          </Button>
          <Button onClick={onCancel}>{labels.cancel}</Button>
        </div>
      </Reveal>
    </form>
  );
}
