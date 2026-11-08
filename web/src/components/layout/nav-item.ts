/** A page of the main navigation. */
export type NavItem = {
  href: string;
  label: string;
  /**
   * Whether the link leads to its own page only, never to the pages under
   * it: the home page of a language (`/fr`) holds every other page of it.
   * The home page of the site (`/`) is always exact.
   */
  exact?: boolean;
};

/** Whether a link of the navigation leads to the page shown, or to one of its children. */
export function isCurrent(
  href: string,
  pathname: string,
  exact = href === '/',
): boolean {
  if (exact) {
    return pathname === href;
  }
  return pathname === href || pathname.startsWith(`${href}/`);
}
