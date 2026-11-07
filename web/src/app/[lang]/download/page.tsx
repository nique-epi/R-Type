import type { Metadata } from 'next';
import { DownloadBlocks } from '@/components/blocks/download/download-blocks';
import { getDictionary } from '@/i18n/get-dictionary';
import { readLocale } from '@/i18n/read-locale';

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/download'>): Promise<Metadata> {
  const lang = await readLocale(params);
  return { title: getDictionary(lang).pages.downloadPage.title };
}

export default async function DownloadPage({
  params,
}: PageProps<'/[lang]/download'>) {
  return <DownloadBlocks lang={await readLocale(params)} />;
}
