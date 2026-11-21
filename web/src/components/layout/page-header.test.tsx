import { render, screen, within } from '@testing-library/react';
import { beforeEach, describe, expect, it, vi } from 'vitest';

import { PageHeader } from './page-header';
import type { SiteAccount } from './site-account';

const pathname = vi.hoisted(() => ({ current: '/fr' }));

vi.mock('next/navigation', () => ({
  usePathname: () => pathname.current,
  useRouter: () => ({ refresh: vi.fn() }),
}));

const ITEMS = [
  { href: '/fr', label: 'Accueil', exact: true },
  { href: '/fr/servers', label: 'Serveurs' },
  { href: '/fr/wiki', label: 'Wiki' },
];

const LABELS = {
  home: 'R-Type, accueil',
  openMenu: 'Ouvrir le menu',
  menu: 'Menu',
  close: 'Fermer le menu',
  navigation: 'Principale',
  logo: 'R-Type',
  download: 'Telecharger',
  signIn: 'Connexion',
  signOut: 'Deconnexion',
};

/** The header itself, for a guest unless given an account: the menu it
 * opens lists the same labels. */
function header(account: SiteAccount | null = null) {
  render(
    <PageHeader
      homeHref="/fr"
      items={ITEMS}
      downloadHref="/fr/download"
      menuFootnote="Linux"
      labels={LABELS}
      account={account}
      signInHref="/fr/signin"
      signOut={async () => undefined}
    />,
  );
  return within(screen.getByRole('banner'));
}

describe('PageHeader', () => {
  beforeEach(() => {
    pathname.current = '/fr';
  });

  it('offers a guest to sign in', () => {
    const banner = header();

    expect(
      banner.getByRole('link', { name: 'Connexion' }).getAttribute('href'),
    ).toBe('/fr/signin');
    expect(banner.queryByRole('button', { name: 'Deconnexion' })).toBeNull();
  });

  it('names the account of a visitor signed in, with a way to sign out', () => {
    const banner = header({ nickname: 'ACE' });

    expect(banner.getByText('ACE')).toBeTruthy();
    expect(banner.getByRole('button', { name: 'Deconnexion' })).toBeTruthy();
    expect(banner.queryByRole('link', { name: 'Connexion' })).toBeNull();
  });

  it('shows no crumb on the home page of a language', () => {
    const banner = header();

    expect(banner.queryByText('Accueil')).toBeNull();
  });

  it('names the section of the page shown beside the logo', () => {
    pathname.current = '/fr/servers';
    const banner = header();

    expect(banner.getByText('Serveurs')).toBeTruthy();
  });

  it('names the section on a page under it', () => {
    pathname.current = '/fr/servers/7';
    const banner = header();

    expect(banner.getByText('Serveurs')).toBeTruthy();
  });

  it('leads the logo to the home page of the language', () => {
    const banner = header();

    expect(
      banner
        .getByRole('link', { name: 'R-Type, accueil' })
        .getAttribute('href'),
    ).toBe('/fr');
  });
});
