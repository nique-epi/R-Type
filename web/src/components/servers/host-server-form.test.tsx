import { fireEvent, render, screen } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import { SERVER_NAME_MAXIMUM } from '@/lib/rtype/server-name';
import { HostServerForm } from './host-server-form';

function renderForm(quotaMessage?: string) {
  const onSubmit = vi.fn();
  render(
    <HostServerForm
      onSubmit={onSubmit}
      onCancel={() => {}}
      quotaMessage={quotaMessage}
      labels={fr.hostForm}
    />,
  );
  return onSubmit;
}

describe('HostServerForm', () => {
  it('refuses an empty name and says why, without sending anything', () => {
    const onSubmit = renderForm();

    fireEvent.click(screen.getByRole('button', { name: fr.hostForm.submit }));

    expect(onSubmit).not.toHaveBeenCalled();
    expect(screen.getByRole('alert').textContent).toBe(
      fr.hostForm.nameRequired,
    );
  });

  it('sends the name without the spaces around it', () => {
    const onSubmit = renderForm();

    fireEvent.change(screen.getByLabelText(fr.hostForm.name), {
      target: { value: '  Nebula Lyon ' },
    });
    fireEvent.click(screen.getByRole('button', { name: fr.hostForm.submit }));

    expect(onSubmit).toHaveBeenCalledWith('Nebula Lyon');
  });

  it('counts the characters against the longest name the master takes', () => {
    renderForm();

    fireEvent.change(screen.getByLabelText(fr.hostForm.name), {
      target: { value: 'Orbit' },
    });

    expect(screen.getByText(`5 / ${SERVER_NAME_MAXIMUM}`)).toBeDefined();
  });

  it('keeps the button off while the quota is reached', () => {
    renderForm('Réessayez dans 38 min.');

    expect(
      (
        screen.getByRole('button', {
          name: fr.hostForm.submit,
        }) as HTMLButtonElement
      ).disabled,
    ).toBe(true);
  });
});
