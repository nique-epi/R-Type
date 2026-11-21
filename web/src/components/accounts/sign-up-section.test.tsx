import { fireEvent, render, screen } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import type { SignUp } from '@/lib/rtype/sign-up';
import { SignUpSection } from './sign-up-section';

const labels = fr.pages.signUpPage;
const PASSWORD = 'correct horse battery staple';

function renderSection(signUp: SignUp) {
  render(
    <SignUpSection
      signUp={signUp}
      homeHref="/fr"
      downloadHref="/fr/download"
      signInHref="/fr/signin"
      labels={labels}
    />,
  );
}

function signUpAsAce() {
  for (const [label, value] of [
    [labels.form.nickname, 'ACE'],
    [labels.form.password, PASSWORD],
    [labels.form.confirmation, PASSWORD],
  ] as const) {
    fireEvent.change(screen.getByLabelText(label), { target: { value } });
  }
  fireEvent.click(screen.getByRole('button', { name: labels.form.submit }));
}

describe('SignUpSection', () => {
  it('leads a pilot who has an account to the sign-in page', () => {
    renderSection(async () => ({ data: { status: 'taken' } }));

    expect(
      screen.getByRole('link', { name: labels.signIn }).getAttribute('href'),
    ).toBe('/fr/signin');
  });

  it('offers to sign in once the account exists', async () => {
    renderSection(async () => ({
      data: { status: 'created', user: { id: 1, nickname: 'ACE' } },
    }));

    signUpAsAce();

    await screen.findByText(labels.created.title);
    expect(
      screen.getByRole('link', { name: labels.signIn }).getAttribute('href'),
    ).toBe('/fr/signin');
  });

  it('welcomes the pilot once the account exists, and drops the form', async () => {
    renderSection(async () => ({
      data: { status: 'created', user: { id: 1, nickname: 'ACE' } },
    }));

    signUpAsAce();

    expect(await screen.findByText(labels.created.title)).toBeTruthy();
    expect(screen.queryByLabelText(labels.form.password)).toBeNull();
  });

  it('says the nickname is taken and keeps what was typed', async () => {
    renderSection(async () => ({ data: { status: 'taken' } }));

    signUpAsAce();

    expect(await screen.findByText(labels.problems.taken.title)).toBeTruthy();
    expect(
      (screen.getByLabelText(labels.form.nickname) as HTMLInputElement).value,
    ).toBe('ACE');
  });

  it('says the master cannot be reached when the action fails', async () => {
    renderSection(() => Promise.reject(new Error('network')));

    signUpAsAce();

    expect(
      await screen.findByText(labels.problems.unreachable.title),
    ).toBeTruthy();
  });

  it('sends the credentials once they pass the form', async () => {
    const signUp = vi.fn<SignUp>(async () => ({ data: { status: 'quota' } }));
    renderSection(signUp);

    signUpAsAce();

    expect(await screen.findByText(labels.problems.quota.title)).toBeTruthy();
    expect(signUp).toHaveBeenCalledWith({
      nickname: 'ACE',
      password: PASSWORD,
    });
  });
});
