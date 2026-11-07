import Link from 'next/link';
import type * as React from 'react';
import type { NavItem } from './nav-item';

export type SiteFooterProps = {
  /** "R-Type · projet Epitech tek3 · v0.1.0". */
  children: React.ReactNode;
  links: readonly NavItem[];
  'aria-label': string;
};

/** The foot of every page: what the site is, and the pages a reader looks for last. */
export function SiteFooter({
  children,
  links,
  'aria-label': label,
}: SiteFooterProps) {
  return (
    <footer
      data-slot="site-footer"
      className="flex flex-wrap justify-between gap-x-8 gap-y-4 border-deep-blue border-t-2 px-8 py-10 small-caps max-md:px-4"
    >
      <span>{children}</span>
      <nav aria-label={label}>
        <ul className="m-0 flex list-none flex-wrap gap-6 p-0">
          {links.map(link => (
            <li key={link.href}>
              <Link
                href={link.href}
                className="text-pale-blue no-underline hover:text-star-white"
              >
                {link.label}
              </Link>
            </li>
          ))}
        </ul>
      </nav>
    </footer>
  );
}
