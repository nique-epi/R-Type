/** A page of the main navigation. */
export type NavItem = {
  href: string;
  label: string;
};

/** Whether a link of the navigation leads to the page shown, or to one of its children. */
export function isCurrent(href: string, pathname: string): boolean {
  if (href === '/') {
    return pathname === '/';
  }
  return pathname === href || pathname.startsWith(`${href}/`);
}
