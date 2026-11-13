'use client';

import { useState, useTransition } from 'react';
import { ArrowLink } from '@/components/primitives/actions/arrow-link';
import { Button } from '@/components/primitives/actions/button';
import { Notice } from '@/components/primitives/status/notice';
import { format } from '@/i18n/format';
import {
  type CreatedServer,
  type CreateServer,
  type ServerCreationOutcome,
  serverConfigLinesOf,
} from '@/lib/rtype/server-creation';
import { SERVER_NAME_MAXIMUM_BYTES } from '@/lib/rtype/server-name';
import { HostServerForm, type HostServerFormLabels } from './host-server-form';
import { ServerKeyPanel, type ServerKeyPanelLabels } from './server-key-panel';

/** An outcome with no server: why the master gave none. */
type CreationProblem = Exclude<ServerCreationOutcome['status'], 'created'>;

export type HostServerSectionLabels = {
  form: HostServerFormLabels;
  key: ServerKeyPanelLabels;
  /** The title and the line of each outcome with no server; `{maximum}` is the longest name. */
  problems: Record<CreationProblem, { title: string; text: string }>;
  configFile: string;
  guide: string;
  another: string;
};

export type HostServerSectionProps = {
  /** Asks the master for a server: the server action of the page. */
  createServer: CreateServer;
  /** The address of the master a host writes in `server.cfg`. */
  masterUrl: string;
  /** The hosting guide of the wiki. */
  guideHref: string;
  labels: HostServerSectionLabels;
};

/**
 * The hosting page under its hero: the form, then the key of the server the
 * master created. The key lives in the state of this component alone, so it
 * is gone with the page; a try that gave no server says why over the form,
 * which keeps the name typed.
 */
export function HostServerSection({
  createServer,
  masterUrl,
  guideHref,
  labels,
}: HostServerSectionProps) {
  const [created, setCreated] = useState<CreatedServer | null>(null);
  const [problem, setProblem] = useState<CreationProblem | null>(null);
  const [pending, startTransition] = useTransition();

  function create(name: string) {
    startTransition(async () => {
      const outcome = await createServer(name).catch(
        (): ServerCreationOutcome => ({ status: 'unreachable' }),
      );
      if (outcome.status === 'created') {
        setCreated(outcome.server);
        setProblem(null);
      } else {
        setProblem(outcome.status);
      }
    });
  }

  if (created) {
    return (
      <ServerKeyPanel
        serverName={created.name}
        serverKey={created.key}
        configFile={labels.configFile}
        configLines={serverConfigLinesOf(masterUrl, created.key)}
        labels={labels.key}
        actions={
          <>
            <Button href={guideHref}>{labels.guide}</Button>
            <Button onClick={() => setCreated(null)}>{labels.another}</Button>
          </>
        }
      />
    );
  }

  return (
    <div className="grid gap-8" data-slot="host-server-section">
      <HostServerForm
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
              {format(labels.problems[problem].text, {
                maximum: SERVER_NAME_MAXIMUM_BYTES,
              })}
            </Notice>
          ) : null
        }
      />
      <ArrowLink href={guideHref}>{labels.guide}</ArrowLink>
    </div>
  );
}
