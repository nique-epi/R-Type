import { PageHero } from '@/components/layout/page-hero';
import { PageMain } from '@/components/primitives/layout/page-main';
import {
  HostServer,
  type HostServerProps,
} from '@/components/servers/host-server';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';

/** The page that hosts a server under its hero: the form, then the key. */
export function HostServerBlocks({
  lang,
  create,
}: {
  lang: Locale;
  /** Creates the server on the master: a server action. */
  create: HostServerProps['create'];
}) {
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.hostPage;

  return (
    <>
      <PageHero eyebrow={copy.eyebrow} title={copy.title} lede={copy.lede} />
      <PageMain className="max-w-190">
        <HostServer
          locale={lang}
          copy={copy}
          formLabels={dictionary.hostForm}
          keyLabels={dictionary.serverKey}
          create={create}
        />
      </PageMain>
    </>
  );
}
