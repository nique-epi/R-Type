import Link from 'next/link';
import type * as React from 'react';

type LinkActionProps = React.ComponentProps<typeof Link>;
type ButtonActionProps = React.ComponentProps<'button'> & { href?: never };

/** A link when it has an `href`, a button otherwise. */
export type ActionProps = LinkActionProps | ButtonActionProps;

/**
 * The element under every clickable primitive: a Next.js link when given an
 * `href` (a page of the site, or an outside page), a button otherwise. A
 * button defaults to `type="button"`, so it never submits a form by surprise.
 */
export function Action(props: ActionProps) {
  if (props.href !== undefined) {
    return <Link {...props} />;
  }
  const { type = 'button', ...rest } = props;
  return <button type={type} {...rest} />;
}
