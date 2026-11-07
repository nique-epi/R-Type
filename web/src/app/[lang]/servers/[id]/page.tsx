import type { Metadata } from 'next';
import { notFound } from 'next/navigation';
import { connection } from 'next/server';
import { ServerSheetBlocks } from '@/components/blocks/servers/server-sheet-blocks';
import { getServerSheet } from '@/hooks/api';
import { format } from '@/i18n/format';
import { getDictionary } from '@/i18n/get-dictionary';
import { readLocale } from '@/i18n/read-locale';

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/servers/[id]'>): Promise<Metadata> {
  await connection();
  const lang = await readLocale(params);
  const { id } = await params;
  const sheet = await getServerSheet(id);

  return {
    title:
      sheet?.server?.name ??
      format(getDictionary(lang).pages.serverPage.pendingName, { number: id }),
  };
}

export default async function ServerPage({
  params,
}: PageProps<'/[lang]/servers/[id]'>) {
  await connection();
  const lang = await readLocale(params);
  const { id } = await params;
  const sheet = await getServerSheet(id);

  if (!sheet) {
    notFound();
  }

  return <ServerSheetBlocks lang={lang} id={id} sheet={sheet} />;
}
