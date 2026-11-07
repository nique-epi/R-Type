import { connection } from 'next/server';
import { HomeBlocks } from '@/components/blocks/home/home-blocks';
import { readLocale } from '@/i18n/read-locale';

export default async function HomePage({ params }: PageProps<'/[lang]'>) {
  await connection();
  return <HomeBlocks lang={await readLocale(params)} />;
}
