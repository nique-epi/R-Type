import { fireEvent, render, screen } from '@testing-library/react';
import { afterEach, describe, expect, it, vi } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import type {
  CreateServer,
  ServerCreationOutcome,
} from '@/lib/rtype/server-creation';
import { HostServerSection } from './host-server-section';

const labels = fr.pages.hostPage;
const KEY = `rtk_${'a1'.repeat(32)}`;
const CREATED: ServerCreationOutcome = {
  status: 'created',
  server: { id: 7, name: 'Nebula Lyon', key: KEY },
};

function renderSection(createServer: CreateServer) {
  render(
    <HostServerSection
      createServer={createServer}
      masterUrl="https://master.test"
      guideHref="/fr/wiki/host"
      labels={labels}
    />,
  );
}

/** Types `name` and sends the form. */
function ask(name: string) {
  fireEvent.change(screen.getByLabelText(labels.form.name), {
    target: { value: name },
  });
  fireEvent.click(screen.getByRole('button', { name: labels.form.submit }));
}

function nameTyped() {
  return (screen.getByLabelText(labels.form.name) as HTMLInputElement).value;
}

describe('HostServerSection', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('asks for a server of the name typed', async () => {
    const createServer = vi.fn<CreateServer>().mockResolvedValue(CREATED);
    renderSection(createServer);

    ask('Nebula Lyon');
    await screen.findByText(KEY);

    expect(createServer).toHaveBeenCalledExactlyOnceWith('Nebula Lyon');
  });

  it('never asks for a server of a name the master would refuse', () => {
    const createServer = vi.fn<CreateServer>();
    renderSection(createServer);

    ask('');
    ask('a'.repeat(33));

    expect(createServer).not.toHaveBeenCalled();
  });

  it('shows the key, says it shows once, and gives the lines of server.cfg', async () => {
    renderSection(async () => CREATED);

    ask('Nebula Lyon');

    expect(await screen.findByText(KEY)).toBeDefined();
    expect(screen.getByText(labels.key.shownOnce)).toBeDefined();
    expect(
      document.querySelector('[data-slot=server-config]')?.textContent,
    ).toBe(`set master-url https://master.test\nset master-key "${KEY}"`);
  });

  it('links to the hosting guide before and after the creation', async () => {
    renderSection(async () => CREATED);
    const guide = () =>
      screen.getByRole('link', { name: labels.guide }).getAttribute('href');

    expect(guide()).toBe('/fr/wiki/host');
    ask('Nebula Lyon');
    await screen.findByText(KEY);

    expect(guide()).toBe('/fr/wiki/host');
  });

  it('copies the key and says so', async () => {
    const writeText = vi.fn().mockResolvedValue(undefined);
    vi.stubGlobal('navigator', { clipboard: { writeText } });
    renderSection(async () => CREATED);
    ask('Nebula Lyon');
    await screen.findByText(KEY);

    fireEvent.click(screen.getByRole('button', { name: labels.key.copy }));

    expect(await screen.findByText(labels.key.copiedLine)).toBeDefined();
    expect(writeText).toHaveBeenCalledExactlyOnceWith(KEY);
  });

  it('says so when the browser refuses the copy', async () => {
    vi.stubGlobal('navigator', {
      clipboard: { writeText: vi.fn().mockRejectedValue(new Error('denied')) },
    });
    renderSection(async () => CREATED);
    ask('Nebula Lyon');
    await screen.findByText(KEY);

    fireEvent.click(screen.getByRole('button', { name: labels.key.copy }));

    expect(await screen.findByText(labels.key.copyFailed)).toBeDefined();
  });

  it.each([
    ['quota', labels.problems.quota],
    ['unreachable', labels.problems.unreachable],
    ['refused', labels.problems.refused],
  ] as const)(
    'says why on %s and keeps the name typed',
    async (status, problem) => {
      renderSection(async () => ({ status }));

      ask('Nebula Lyon');

      expect((await screen.findByRole('alert')).textContent).toBe(
        `${problem.title}${problem.text}`,
      );
      expect(nameTyped()).toBe('Nebula Lyon');
    },
  );

  it('gives the quota and the silent master two different messages', () => {
    expect(labels.problems.quota.text).not.toBe(
      labels.problems.unreachable.text,
    );
  });

  it('says the master is unreachable when the action itself fails', async () => {
    renderSection(async () => {
      throw new Error('network');
    });

    ask('Nebula Lyon');

    expect((await screen.findByRole('alert')).textContent).toContain(
      labels.problems.unreachable.text,
    );
    expect(nameTyped()).toBe('Nebula Lyon');
  });

  it('forgets the key when another server is asked for', async () => {
    renderSection(async () => CREATED);
    ask('Nebula Lyon');
    await screen.findByText(KEY);

    fireEvent.click(screen.getByRole('button', { name: labels.another }));

    expect(screen.queryByText(KEY)).toBeNull();
    expect(nameTyped()).toBe('');
  });
});
