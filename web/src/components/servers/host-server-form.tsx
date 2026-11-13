'use client';

import type * as React from 'react';
import { useState } from 'react';
import { Button } from '@/components/primitives/actions/button';
import { TextField } from '@/components/primitives/inputs/text-field';
import { Notice } from '@/components/primitives/status/notice';
import { PanelTitle } from '@/components/primitives/surfaces/panel';
import { format } from '@/i18n/format';
import {
  SERVER_NAME_MAXIMUM_BYTES,
  type ServerNameProblem,
  serverNameProblemOf,
  serverNameSizeOf,
} from '@/lib/rtype/server-name';

export type HostServerFormLabels = {
  title: string;
  name: string;
  placeholder: string;
  /** "{length} / {maximum}". */
  counter: string;
  nameRequired: string;
  /** "Ce nom est trop long : {maximum} caractères au plus". */
  nameTooLong: string;
  shownOnceTitle: string;
  shownOnce: string;
  submit: string;
  /** The button while the master is asked. */
  pending: string;
};

export type HostServerFormProps = {
  /** Called with a name the master takes, the blanks around it left out. */
  onSubmit: (name: string) => void;
  /** The master is being asked: the button waits. */
  pending?: boolean;
  /** Why the last try gave no server, shown over the buttons. */
  problem?: React.ReactNode;
  labels: HostServerFormLabels;
};

/**
 * The form that creates a game server on the master: its name, and the
 * warning that its key will show once. A name the master would refuse,
 * empty or over 32 bytes, is refused here and nothing is sent. The name
 * typed stays whatever the answer, since the form keeps it.
 */
export function HostServerForm({
  onSubmit,
  pending = false,
  problem,
  labels,
}: HostServerFormProps) {
  const [name, setName] = useState('');
  const [refused, setRefused] = useState<ServerNameProblem | null>(null);
  const errors: Record<ServerNameProblem, string> = {
    empty: labels.nameRequired,
    tooLong: format(labels.nameTooLong, {
      maximum: SERVER_NAME_MAXIMUM_BYTES,
    }),
  };

  function submit(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();
    const found = serverNameProblemOf(name);
    setRefused(found);
    if (found === null) {
      onSubmit(name.trim());
    }
  }

  return (
    <form
      onSubmit={submit}
      noValidate
      className="grid gap-6"
      data-slot="host-server-form"
    >
      <PanelTitle className="m-0">{labels.title}</PanelTitle>
      <TextField
        label={labels.name}
        value={name}
        onChange={event => {
          setName(event.target.value);
          setRefused(null);
        }}
        placeholder={labels.placeholder}
        autoComplete="off"
        error={refused ? errors[refused] : undefined}
        help={format(labels.counter, {
          length: serverNameSizeOf(name.trim()),
          maximum: SERVER_NAME_MAXIMUM_BYTES,
        })}
        required
      />
      <Notice title={labels.shownOnceTitle}>{labels.shownOnce}</Notice>
      {problem}
      <div className="flex flex-wrap items-center gap-4">
        <Button type="submit" variant="solid" disabled={pending}>
          {pending ? labels.pending : labels.submit}
        </Button>
      </div>
    </form>
  );
}
