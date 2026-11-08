'use client';

import { usePathname } from 'next/navigation';
import { isCurrent } from './nav-item';
import { SiteHeader, type SiteHeaderProps } from './site-header';

/**
 * The header of a site whose pages are the items of its navigation: the
 * crumb beside the logo is the label of the item whose page is shown, and
 * the home page has none.
 */
export function PageHeader({
  items,
  ...props
}: Omit<SiteHeaderProps, 'crumb'>) {
  const pathname = usePathname();
  const section = items.find(
    item => !item.exact && isCurrent(item.href, pathname),
  );

  return <SiteHeader {...props} items={items} crumb={section?.label} />;
}
