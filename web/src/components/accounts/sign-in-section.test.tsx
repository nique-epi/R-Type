import { render, screen } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import { SignInSection } from './sign-in-section';

vi.mock('next/navigation', () => ({
  useRouter: () => ({ replace: vi.fn(), refresh: vi.fn() }),
}));

const labels = fr.pages.signInPage;

describe('SignInSection', () => {
  it('leads a visitor with no account to the sign-up page', () => {
    render(
      <SignInSection
        signIn={async () => ({ data: { status: 'wrong' } })}
        nextHref="/fr"
        signUpHref="/fr/signup"
        labels={labels}
      />,
    );

    expect(
      screen.getByRole('link', { name: labels.signUp }).getAttribute('href'),
    ).toBe('/fr/signup');
  });
});
