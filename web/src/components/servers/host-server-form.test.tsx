import { fireEvent, render, screen } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import { HostServerForm } from './host-server-form';

const labels = fr.pages.hostPage.form;

function renderForm(pending = false) {
  const onSubmit = vi.fn();
  render(
    <HostServerForm onSubmit={onSubmit} pending={pending} labels={labels} />,
  );
  return onSubmit;
}

function typeName(name: string) {
  fireEvent.change(screen.getByLabelText(labels.name), {
    target: { value: name },
  });
}

function submit() {
  fireEvent.click(screen.getByRole('button', { name: labels.submit }));
}

describe('HostServerForm', () => {
  it('refuses an empty name and says why, without sending anything', () => {
    const onSubmit = renderForm();

    submit();

    expect(onSubmit).not.toHaveBeenCalled();
    expect(screen.getByRole('alert').textContent).toBe(labels.nameRequired);
  });

  it('refuses a name of 33 characters and says why, without sending anything', () => {
    const onSubmit = renderForm();

    typeName('a'.repeat(33));
    submit();

    expect(onSubmit).not.toHaveBeenCalled();
    expect(screen.getByRole('alert').textContent).toContain('32');
  });

  it('sends a name of 32 characters', () => {
    const onSubmit = renderForm();

    typeName('a'.repeat(32));
    submit();

    expect(onSubmit).toHaveBeenCalledWith('a'.repeat(32));
  });

  it('sends the name without the spaces around it', () => {
    const onSubmit = renderForm();

    typeName('  Nebula Lyon ');
    submit();

    expect(onSubmit).toHaveBeenCalledWith('Nebula Lyon');
  });

  it('counts the name against the longest one the master takes', () => {
    renderForm();

    typeName('Orbit');

    expect(screen.getByText('5 / 32')).toBeDefined();
  });

  it('says before the creation that the key shows once', () => {
    renderForm();

    expect(screen.getByText(labels.shownOnce)).toBeDefined();
  });

  it('keeps the button off while the master is asked', () => {
    renderForm(true);

    expect(
      (
        screen.getByRole('button', {
          name: labels.pending,
        }) as HTMLButtonElement
      ).disabled,
    ).toBe(true);
  });
});
