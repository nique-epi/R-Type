'use client';

import Link from 'next/link';
import type * as React from 'react';
import { useState } from 'react';
import { SignedInAccount } from '@/components/accounts/signed-in-account';
import { Button } from '@/components/primitives/actions/button';
import { IconButton } from '@/components/primitives/actions/icon-button';
import { PixelIcon } from '@/components/primitives/icons/pixel-icon';
import { Logo } from '@/components/sprites/logo';
import type { NavItem } from './nav-item';
import type { SiteAccountProps } from './site-account';
import { SiteMenu, type SiteMenuLabels } from './site-menu';

export type SiteHeaderLabels = SiteMenuLabels & {
  home: string;
  openMenu: string;
};

export type SiteHeaderProps = {
  /** Where the logo leads: the home page, `/` by default. */
  homeHref?: string;
  /** Where the reader is, beside the logo: "Serveurs". Hidden on a phone. */
  crumb?: React.ReactNode;
  items: readonly NavItem[];
  downloadHref: string;
  menuFootnote: React.ReactNode;
  labels: SiteHeaderLabels;
} & SiteAccountProps;

/**
 * The header of every page: a pill that floats over the top of the page and
 * stays there as it scrolls, with the logo, the account of the visitor, the
 * download button and the button that opens the menu. The account is *Sign
 * in* for a guest, the nickname and *Sign out* once signed in; a phone finds
 * it in the menu instead.
 */
export function SiteHeader({
  homeHref = '/',
  crumb,
  items,
  downloadHref,
  menuFootnote,
  labels,
  account,
  signInHref,
  signOut,
}: SiteHeaderProps) {
  const [menuOpen, setMenuOpen] = useState(false);

  return (
    <>
      <header
        data-slot="site-header"
        className="sticky top-3 z-20 mx-4 mt-3 flex items-center gap-4 rounded-full bg-sky/88 py-2.5 pr-3 pl-7 shadow-float backdrop-blur-md max-md:mx-2 max-md:mt-2 max-md:py-2 max-md:pr-2 max-md:pl-5"
      >
        <Link href={homeHref} aria-label={labels.home} className="block">
          <Logo alt="" priority />
        </Link>
        {crumb ? (
          <span className="small-caps text-pale-blue max-md:hidden">
            {crumb}
          </span>
        ) : null}
        <span className="flex-1" />
        {account ? (
          <SignedInAccount
            nickname={account.nickname}
            signOutLabel={labels.signOut}
            signOut={signOut}
            className="max-md:hidden"
          />
        ) : (
          <Button href={signInHref} size="sm" className="max-md:hidden">
            {labels.signIn}
          </Button>
        )}
        <Button href={downloadHref} variant="solid">
          {labels.download}
        </Button>
        <IconButton
          aria-label={labels.openMenu}
          aria-expanded={menuOpen}
          aria-haspopup="dialog"
          onClick={() => setMenuOpen(true)}
        >
          <PixelIcon name="menu" size={28} />
        </IconButton>
      </header>
      <SiteMenu
        open={menuOpen}
        onClose={() => setMenuOpen(false)}
        items={items}
        downloadHref={downloadHref}
        footnote={menuFootnote}
        labels={labels}
        account={account}
        signOut={signOut}
      />
    </>
  );
}
