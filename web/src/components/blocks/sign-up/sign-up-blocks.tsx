import { SignUpSection } from '@/components/accounts/sign-up-section';
import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { StarLayers } from '@/components/sky/star-layers';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { pathOf } from '@/lib/site-paths';
import { signUpAction } from './sign-up.action';

/**
 * The sign-up page under its hero, on the sky of the inner pages: the form
 * that creates an account on the master, then the welcome of the pilot.
 */
export function SignUpBlocks({ lang }: { lang: Locale }) {
  const copy = getDictionary(lang).pages.signUpPage;

  return (
    <div className="relative isolate">
      <StarLayers className="-z-10" />
      <PageHero eyebrow={copy.eyebrow} title={copy.title} lede={copy.lede} />
      <PageBody compact>
        <SignUpSection
          signUp={signUpAction}
          homeHref={pathOf(lang, 'home')}
          downloadHref={pathOf(lang, 'download')}
          signInHref={pathOf(lang, 'signIn')}
          labels={copy}
        />
      </PageBody>
    </div>
  );
}
