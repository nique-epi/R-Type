import { redirect } from 'next/navigation';
import { LinkSection } from '@/components/accounts/link-section';
import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { Button } from '@/components/primitives/actions/button';
import { TextField } from '@/components/primitives/inputs/text-field';
import { Notice } from '@/components/primitives/status/notice';
import { StarLayers } from '@/components/sky/star-layers';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { api } from '@/lib/api';
import { userCodeOf } from '@/lib/rtype/device-link';
import { readSessionToken } from '@/lib/session/session-cookie';
import { pathOf } from '@/lib/site-paths';
import { signOutAction } from '../sign-in/sign-out.action';
import { decideLinkAction } from './link.action';

/** The path of the page `/link` in `lang`, with its code when it has one. */
function linkPathOf(lang: Locale, userCode: string | null) {
  const path = pathOf(lang, 'link');
  return userCode ? `${path}?code=${encodeURIComponent(userCode)}` : path;
}

/** Sends a visitor with no session to the sign-in page, which leads back. */
function signInFirst(lang: Locale, userCode: string | null): never {
  redirect(
    `${pathOf(lang, 'signIn')}?next=${encodeURIComponent(linkPathOf(lang, userCode))}`,
  );
}

/**
 * The page a game sends its player to, `/link?code=KQFB-WZPM`: a visitor
 * signs in first, then sees what game asks and when, and connects or refuses
 * it. Without a code, or with one no link holds, it asks for the code the
 * game shows, so a player may validate from another device.
 */
export async function LinkBlocks({
  lang,
  code,
}: {
  lang: Locale;
  code: string | undefined;
}) {
  const copy = getDictionary(lang).pages.linkPage;
  const userCode = userCodeOf(code);
  const token = await readSessionToken();
  if (!token) {
    signInFirst(lang, userCode);
  }
  const read = userCode
    ? await api.sessions.readDeviceLink({ userCode, token })
    : null;
  if (read?.error === 'UNAUTHORIZED') {
    signInFirst(lang, userCode);
  }

  return (
    <div className="relative isolate">
      <StarLayers className="-z-10" />
      <PageHero eyebrow={copy.eyebrow} title={copy.title} lede={copy.lede} />
      <PageBody compact>
        {read?.data ? (
          <LinkSection
            link={read.data}
            decide={decideLinkAction}
            signOut={signOutAction}
            locale={lang}
            labels={copy.link}
          />
        ) : (
          <form
            action={pathOf(lang, 'link')}
            method="get"
            className="grid gap-6"
            data-slot="link-code-form"
          >
            {code !== undefined ? (
              <Notice role="alert" tone="warning" title={copy.unknown.title}>
                {read?.error === 'NOT_FOUND' || !userCode
                  ? copy.unknown.text
                  : copy.unreachable}
              </Notice>
            ) : null}
            <TextField
              label={copy.codeForm.label}
              name="code"
              placeholder={copy.codeForm.placeholder}
              autoComplete="off"
              autoCapitalize="characters"
              spellCheck={false}
              defaultValue={code ?? ''}
              required
            />
            <div>
              <Button type="submit" variant="solid">
                {copy.codeForm.submit}
              </Button>
            </div>
          </form>
        )}
      </PageBody>
    </div>
  );
}
