'use client';

import { useRouter } from 'next/navigation';
import type * as React from 'react';
import { useTransition } from 'react';
import { Button } from '@/components/primitives/actions/button';
import { cn } from '@/lib/utils';

export type SignedInAccountProps = Omit<
  React.ComponentProps<'div'>,
  'children'
> & {
  nickname: string;
  /** The words of the button: "Deconnexion". */
  signOutLabel: string;
  /** Closes the session of the visitor; the page is read again after it. */
  signOut: () => Promise<unknown>;
};

/**
 * The account of a visitor signed in, in the header or the menu: the
 * nickname, then the button that signs out. Signing out closes the session,
 * then reads the page again, whose header then offers to sign in; a master
 * out of reach changes nothing, since the action forgets the cookie all the
 * same.
 */
export function SignedInAccount({
  nickname,
  signOutLabel,
  signOut,
  className,
  ...props
}: SignedInAccountProps) {
  const router = useRouter();
  const [pending, startTransition] = useTransition();

  function leave() {
    startTransition(async () => {
      await signOut().catch(() => undefined);
      router.refresh();
    });
  }

  return (
    <div className={cn('flex items-center gap-3', className)} {...props}>
      <span className="small-caps text-pale-blue">{nickname}</span>
      <Button size="sm" disabled={pending} onClick={leave}>
        {signOutLabel}
      </Button>
    </div>
  );
}
