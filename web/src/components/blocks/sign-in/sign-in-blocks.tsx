import { SignInSection } from '@/components/accounts/sign-in-section';
import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { StarLayers } from '@/components/sky/star-layers';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { nextPathOf } from '@/lib/rtype/sign-in';
import { pathOf } from '@/lib/site-paths';
import { signInAction } from './sign-in.action';

/**
 * The sign-in page under its hero, on the sky of the inner pages: the form
 * that opens a session on the master, then the page `next` names, the home
 * page when it names none of this site.
 */
export function SignInBlocks({
  lang,
  next,
}: {
  lang: Locale;
  next: string | undefined;
}) {
  const copy = getDictionary(lang).pages.signInPage;

  return (
    <div className="relative isolate">
      <StarLayers className="-z-10" />
      <PageHero eyebrow={copy.eyebrow} title={copy.title} lede={copy.lede} />
      <PageBody compact>
        <SignInSection
          signIn={signInAction}
          nextHref={nextPathOf(next, pathOf(lang, 'home'))}
          labels={copy}
        />
      </PageBody>
    </div>
  );
}
