import type { User } from '@/lib/api/schemas';

/** The account the visitor is signed in with, as the header shows it: its
 * nickname only, so its id and role never reach the browser. */
export type SiteAccount = Pick<User, 'nickname'>;

/** How the header and the menu reach the account of the visitor. */
export type SiteAccountProps = {
  /** The account of the visitor, `null` for a guest. */
  account: SiteAccount | null;
  /** Where *Sign in* leads a guest. */
  signInHref: string;
  /** Closes the session of the visitor. */
  signOut: () => Promise<unknown>;
};
