import type { Metadata } from 'next';
import { PageHero } from '@/components/layout/page-hero';
import { PageMain } from '@/components/primitives/layout/page-main';
import { HostServer } from '@/components/servers/host-server';
import { getDictionary } from '@/i18n/get-dictionary';
import { readLocale } from '@/i18n/read-locale';
import { createServerAction } from './actions';

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/servers/new'>): Promise<Metadata> {
  const lang = await readLocale(params);
  return { title: getDictionary(lang).pages.hostPage.title };
}

export default async function HostServerPage({
  params,
}: PageProps<'/[lang]/servers/new'>) {
  const lang = await readLocale(params);
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
          create={createServerAction}
        />
      </PageMain>
    </>
  );
}
