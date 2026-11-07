'use client';

import { useRouter } from 'next/navigation';
import { useState, useTransition } from 'react';
import { Button } from '@/components/primitives/actions/button';
import { Notice } from '@/components/primitives/status/notice';
import type { Locale } from '@/i18n/locales';
import type { PageCopy } from '@/i18n/page-copy';
import type { CreationOutcome } from '@/lib/rtype/creation-outcome';
import { pathOf, serverPathOf } from '@/lib/site-paths';
import { HostServerForm, type HostServerFormLabels } from './host-server-form';
import { ServerKeyPanel, type ServerKeyPanelLabels } from './server-key-panel';

/** How much of its key the `server.cfg` line shows: `rtk_` and five characters, then the last four. */
const KEY_HEAD_LENGTH = 9;
const KEY_TAIL_LENGTH = 4;

export type HostServerProps = {
  locale: Locale;
  copy: PageCopy['hostPage'];
  formLabels: HostServerFormLabels;
  keyLabels: ServerKeyPanelLabels;
  /** Creates the server on the master (a server action): it never throws, it says what happened. */
  create: (name: string) => Promise<CreationOutcome>;
};

/** The line of `server.cfg` that sets the key, the key cut short as the key panel prints it. */
export function configLineOf(key: string): string {
  return `set master-key "${key.slice(0, KEY_HEAD_LENGTH)}…${key.slice(-KEY_TAIL_LENGTH)}"`;
}

/**
 * Hosting a server: the form, then, once the master created it, the panel
 * that shows its key this once. A quota or a refusal leaves the form where it
 * is, with what went wrong written above or in it.
 */
export function HostServer({
  locale,
  copy,
  formLabels,
  keyLabels,
  create,
}: HostServerProps) {
  const router = useRouter();
  const [outcome, setOutcome] = useState<CreationOutcome | null>(null);
  const [pending, startTransition] = useTransition();

  if (outcome?.status === 'created') {
    return (
      <ServerKeyPanel
        serverName={outcome.name}
        serverKey={outcome.key}
        configFile={copy.configFile}
        configLine={configLineOf(outcome.key)}
        labels={keyLabels}
        actions={
          <>
            <Button href={serverPathOf(locale, String(outcome.id))}>
              {copy.openServer}
            </Button>
            <Button onClick={() => setOutcome(null)}>{copy.another}</Button>
          </>
        }
      />
    );
  }

  const failure =
    outcome?.status === 'unreachable'
      ? copy.unreachable
      : outcome?.status === 'refused'
        ? copy.refused
        : null;

  return (
    <div className="grid gap-6">
      {failure ? (
        <Notice tone="warning" title={copy.errorTitle} role="alert">
          {failure}
        </Notice>
      ) : null}
      <HostServerForm
        labels={formLabels}
        pending={pending}
        quotaMessage={outcome?.status === 'quota' ? copy.quota : undefined}
        onCancel={() => router.push(pathOf(locale, 'servers'))}
        onSubmit={name =>
          startTransition(async () => {
            setOutcome(await create(name));
          })
        }
      />
    </div>
  );
}
