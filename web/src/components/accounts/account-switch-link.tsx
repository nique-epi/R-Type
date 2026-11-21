import { ArrowLink } from '@/components/primitives/actions/arrow-link';

export type AccountSwitchLinkProps = {
  /** The question before the link: "Déjà un compte ?". */
  prompt: string;
  href: string;
  label: string;
};

/**
 * The line under a form of the account that leads to the other one: from
 * signing up to signing in, and back.
 */
export function AccountSwitchLink({
  prompt,
  href,
  label,
}: AccountSwitchLinkProps) {
  return (
    <p className="m-0 flex flex-wrap items-center gap-3 text-pale-blue">
      {prompt}
      <ArrowLink href={href}>{label}</ArrowLink>
    </p>
  );
}
