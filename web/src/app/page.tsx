import { connection } from 'next/server';
import { AutoRefresh } from '@/components/auto-refresh';
import { MasterStatus } from '@/components/master/master-status';
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from '@/components/primitives/card';

const STATUS_REFRESH_INTERVAL_MS = 5_000;

export default async function HomePage() {
  await connection();

  return (
    <main className="mx-auto flex w-full max-w-xl flex-1 flex-col gap-6 p-6">
      <h1 className="font-semibold text-2xl tracking-tight">R-Type</h1>
      <Card>
        <CardHeader>
          <CardTitle>
            <h2>Master</h2>
          </CardTitle>
          <CardDescription>
            The service that lists the game servers.
          </CardDescription>
        </CardHeader>
        <CardContent>
          <MasterStatus />
        </CardContent>
      </Card>
      <AutoRefresh intervalMs={STATUS_REFRESH_INTERVAL_MS} />
    </main>
  );
}
