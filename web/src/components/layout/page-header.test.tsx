import { render, screen, within } from '@testing-library/react';
import { beforeEach, describe, expect, it, vi } from 'vitest';

import { PageHeader } from './page-header';

const pathname = vi.hoisted(() => ({ current: '/fr' }));

vi.mock('next/navigation', () => ({ usePathname: () => pathname.current }));

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
};

/** The header itself: the menu it opens lists the same labels. */
function header() {
  render(
    <PageHeader
      homeHref="/fr"
      items={ITEMS}
      downloadHref="/fr/download"
      menuFootnote="Linux"
      labels={LABELS}
    />,
  );
  return within(screen.getByRole('banner'));
}

describe('PageHeader', () => {
  beforeEach(() => {
    pathname.current = '/fr';
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
