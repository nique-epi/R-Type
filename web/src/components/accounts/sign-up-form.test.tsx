import { fireEvent, render, screen, waitFor } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import { SignUpForm } from './sign-up-form';

const labels = fr.pages.signUpPage.form;
const PASSWORD = 'correct horse battery staple';

function renderForm() {
  const onSubmit = vi.fn();
  render(<SignUpForm onSubmit={onSubmit} labels={labels} />);
  return onSubmit;
}

function type(label: string, value: string) {
  fireEvent.change(screen.getByLabelText(label), { target: { value } });
}

function fill(nickname: string, password: string, confirmation = password) {
  type(labels.nickname, nickname);
  type(labels.password, password);
  type(labels.confirmation, confirmation);
}

function submit() {
  fireEvent.click(screen.getByRole('button', { name: labels.submit }));
}

describe('SignUpForm', () => {
  it('sends the nickname without the blanks around it, and the password', async () => {
    const onSubmit = renderForm();

    fill('  ACE ', PASSWORD);
    submit();

    await waitFor(() =>
      expect(onSubmit).toHaveBeenCalledWith({
        nickname: 'ACE',
        password: PASSWORD,
      }),
    );
  });

  it('refuses an empty nickname and says why, without sending anything', async () => {
    const onSubmit = renderForm();

    fill('', PASSWORD);
    submit();

    expect((await screen.findByRole('alert')).textContent).toBe(
      labels.problems.nicknameRequired,
    );
    expect(onSubmit).not.toHaveBeenCalled();
  });

  it('refuses a password of 7 characters and names the minimum', async () => {
    const onSubmit = renderForm();

    fill('ACE', 'a'.repeat(7));
    submit();

    expect((await screen.findByRole('alert')).textContent).toContain('8');
    expect(onSubmit).not.toHaveBeenCalled();
  });

  it('refuses a confirmation that differs, without sending anything', async () => {
    const onSubmit = renderForm();

    fill('ACE', PASSWORD, `${PASSWORD}!`);
    submit();

    expect((await screen.findByRole('alert')).textContent).toBe(
      labels.problems.confirmationDifferent,
    );
    expect(onSubmit).not.toHaveBeenCalled();
  });

  it('hides the passwords and tells the browser they are new ones', () => {
    renderForm();

    for (const label of [labels.password, labels.confirmation]) {
      const field = screen.getByLabelText(label);
      expect(field.getAttribute('type')).toBe('password');
      expect(field.getAttribute('autocomplete')).toBe('new-password');
    }
    expect(
      screen.getByLabelText(labels.nickname).getAttribute('autocomplete'),
    ).toBe('username');
  });

  it('counts the nickname in bytes, as the master does', () => {
    renderForm();

    type(labels.nickname, 'Élo');

    expect(screen.getByText('4 / 16')).toBeTruthy();
  });
});
