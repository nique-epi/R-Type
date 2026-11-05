"""Records the datagrams of a real game, for the Huffman table of the server.

Usage: python3 capture_corpus.py <port> <players> <seconds> <output file>

Start the real server first, on <port>, for instance:
  ./r-type_server -p 4301 --seed 1 --no-log-file --no-log-stderr
The script plays <players> scripted pilots through the whole protocol (cookie
handshake, ready, inputs at 60 per second) and writes every datagram of both
directions, as records of: seconds (f64), direction (u8, 0 client to server,
1 server to client), pilot (u8), size (u16), then the datagram bytes. All
numbers are big-endian.
"""
import asyncio, random, struct, sys, time

PORT = int(sys.argv[1])
PLAYERS = int(sys.argv[2])
DURATION = float(sys.argv[3])
OUT = sys.argv[4]
random.seed(PORT)

HDR = ">IBBQHHI"
records = []  # (t, direction, peer, bytes)


class Client(asyncio.DatagramProtocol):
    def __init__(self, index, done):
        self.index, self.done = index, done
        self.token = 0
        self.sequence = 0
        self.newest = None
        self.bits = 0
        self.state = "connecting"
        self.cookie = 0
        self.input_number = 0
        self.actions = 0
        self.loaded_sent = False
        self.got_state = False

    def connection_made(self, transport):
        self.transport = transport
        asyncio.ensure_future(self.run())

    def build(self, messages, guaranteed=False):
        flags = 0
        ack = 0
        if self.newest is not None:
            flags |= 1
            ack = self.newest
        if guaranteed:
            flags |= 2
        header = struct.pack(HDR, 0x52545950, 2, flags, self.token,
                             self.sequence, ack, self.bits)
        self.sequence = (self.sequence + 1) & 0xFFFF
        body = b"".join(struct.pack(">BH", t, len(p)) + p for t, p in messages)
        return header + body

    def send(self, messages):
        data = self.build(messages)
        records.append((time.monotonic(), "c2s", self.index, data))
        self.transport.sendto(data)

    def datagram_received(self, data, addr):
        records.append((time.monotonic(), "s2c", self.index, data))
        if len(data) < 22:
            return
        _, _, flags, token, seq, _, _ = struct.unpack(HDR, data[:22])
        if token and not self.token:
            self.token = token
        if self.newest is None:
            self.newest, self.bits = seq, 0
        else:
            diff = (seq - self.newest) & 0xFFFF
            if 0 < diff < 32768:
                self.bits = ((self.bits << diff) | (1 << (diff - 1))) & 0xFFFFFFFF
                self.newest = seq
            elif diff and diff >= 32768:
                back = (self.newest - seq) & 0xFFFF
                if 1 <= back <= 32:
                    self.bits |= 1 << (back - 1)
        pos = 22
        while pos + 3 <= len(data):
            t, size = struct.unpack(">BH", data[pos:pos + 3])
            payload = data[pos + 3:pos + 3 + size]
            pos += 3 + size
            if t == 16:
                self.cookie = struct.unpack(">Q", payload)[0]
            elif t == 2:
                self.state = "lobby"
            elif t == 8 and self.state in ("lobby", "ready"):
                self.state = "loading"
            elif t == 10:
                self.got_state = True
                self.state = "playing"

    async def run(self):
        nickname = f"Pilot{self.index}".encode()
        request = lambda: self.send([(1, struct.pack(">Q", self.cookie) + bytes([len(nickname)]) + nickname)])
        for _ in range(20):
            request()
            await asyncio.sleep(0.1)
            if self.state != "connecting":
                break
        await asyncio.sleep(0.3)
        for _ in range(10):
            self.send([(6, bytes([1]))])
            await asyncio.sleep(0.2)
            if self.state in ("loading", "playing"):
                break
        start = time.monotonic()
        next_time = start
        while time.monotonic() - start < DURATION:
            next_time += 1 / 60
            if self.state in ("loading", "playing"):
                if random.random() < 0.08:
                    self.actions = random.choice([0, 1, 2, 4, 8, 16, 16 | 8, 16 | 1, 16 | 2, 8 | 1, 2 | 4])
                self.input_number += 1
                messages = [(9, struct.pack(">IB", self.input_number, self.actions))]
                if not self.got_state:
                    messages.append((17, b""))
                self.send(messages)
            else:
                self.send([(5, b"")])
            await asyncio.sleep(max(0, next_time - time.monotonic()))
        self.send([(4, b"")])
        self.done.set()


async def main():
    loop = asyncio.get_running_loop()
    events = []
    for i in range(PLAYERS):
        done = asyncio.Event()
        events.append(done)
        await loop.create_datagram_endpoint(lambda i=i, d=done: Client(i, d), remote_addr=("127.0.0.1", PORT))
        await asyncio.sleep(0.05)
    for e in events:
        await e.wait()
    await asyncio.sleep(0.3)


asyncio.run(main())
t0 = records[0][0]
with open(OUT, "wb") as f:
    for t, d, peer, data in records:
        f.write(struct.pack(">dBBH", t - t0, 0 if d == "c2s" else 1, peer, len(data)) + data)
print(len(records), "datagrams", sum(len(r[3]) for r in records), "bytes")
