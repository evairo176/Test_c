# Event Queue Bahasa C (ThreeSevenLab)

Implementasi **Event Queue** dengan **Singly Linked List** dan mekanisme
**FIFO (First In, First Out)** menggunakan bahasa **C murni** (bukan C++/C#).

## Struktur File

| File | Isi |
|---|---|
| `event_queue.h` | Deklarasi `struct Event`, `EventNode`, `EventQueue` + prototipe fungsi |
| `event_queue.c` | Implementasi PushEvent, PopEvent, GetEventCount, ClearEvent |
| `main.c` | Pengujian seluruh fungsi + skenario edge case |
| `README.md` | Dokumentasi ini |

## Kompilasi dan Jalankan

```bash
gcc -Wall -Wextra -Wpedantic -std=c11 main.c event_queue.c -o event_queue
./event_queue
```

## Cara Menjalankan (panduan lengkap)

### 1. Clone dan masuk folder

```bash
git clone https://github.com/evairo176/Test_c.git
cd Test_c
```

### 2. Kompilasi dengan GCC

```bash
gcc -Wall -Wextra -Wpedantic -std=c11 main.c event_queue.c -o event_queue
```

- GCC harus terpasang: `sudo apt install gcc` (Ubuntu/Debian),
  `xcode-select --install` (macOS), atau MinGW-w64 (Windows).
- Kalau sukses, tidak ada output sama sekali (0 error, 0 warning).
- Di Windows pakai MinGW: perintah `gcc` sama, hasilnya
  `event_queue.exe`.

### 3. Jalankan program

```bash
./event_queue
```

PowerShell/CMD: `event_queue.exe`.

### 4. Contoh output

```
[1] Queue kosong
  GetEventCount = 0 (harap 0)
  PopEvent saat kosong = 0 (harap 0)

[2] Push satu event
  Push ok, GetEventCount = 1 (harap 1)

[3] Push beberapa event (urutan FIFO harus terjaga)
  GetEventCount = 4 (harap 4)

[4] Pop berurutan (FIFO)
  pop -> id=1 name="EVENT_INIT"
  pop -> id=2 name="EVENT_LOGIN"
  pop -> id=3 name="EVENT_ORDER"
  pop -> id=4 name="EVENT_PAYMENT"
  GetEventCount = 0 (harap 0)

[5] Pop saat kosong = 0 (harap 0)

[7a] ClearEvent queue kosong (harus aman)
[7b] ClearEvent queue berisi 3 event
  Sebelum clear GetEventCount = 3 (harap 3)
  Sesudah clear GetEventCount = 0 (harap 0)

[8] Push lagi setelah ClearEvent
  Push ok, GetEventCount = 1 (harap 1)
  pop -> id=20 name="EVENT_AFTER_CLEAR"

[9] Final GetEventCount = 0 (harap 0, semua memori di-free)
SELESAI - semua skenario lolos tanpa crash.
```

### 5. Pemeriksaan memori (opsional, direkomendasikan)

```bash
# AddressSanitizer - deteksi leak/double-free/use-after-free
gcc -Wall -Wextra -Wpedantic -std=c11 -fsanitize=address,undefined -g main.c event_queue.c -o event_queue_asan
ASAN_OPTIONS=detect_leaks=1 ./event_queue_asan

# atau valgrind kalau tersedia
valgrind --leak-check=full ./event_queue
```

Keduanya harus bersih: ASAN exit 0, valgrind melaporkan
`in use at exit: 0 bytes`.

Kompilasi dengan AddressSanitizer (opsional, untuk deteksi memory error):

```bash
gcc -Wall -Wextra -Wpedantic -std=c11 -fsanitize=address,undefined -g main.c event_queue.c -o event_queue_asan
./event_queue_asan
```

## API

- `int PushEvent(EventQueue *queue, const Event *event)` - tambah event ke
  belakang (tail). Return 1 sukses, 0 gagal (NULL/malloc gagal).
- `int PopEvent(EventQueue *queue, Event *out)` - ambil event terdepan (head),
  hapus node, salin ke `*out`. Return 1 sukses, 0 kalau queue kosong.
- `size_t GetEventCount(const EventQueue *queue)` - jumlah event tersimpan.
- `void ClearEvent(EventQueue *queue)` - hapus semua event, free semua node.

## Desain Singly Linked List & Alur FIFO

Queue memegang dua pointer: **head** (node paling lama) dan **tail** (node
paling baru) plus **count**.

```
PushEvent:  tail->next = node_baru; tail = node_baru;   (O(1))
PopEvent:   node = head; head = head->next; free(node); (O(1))
            kalau head jadi NULL, tail ikut NULL (cegah dangling)
```

Konsistensi pointer dijaga di semua jalur:

- Push ke queue kosong: head = tail = node baru.
- Pop terakhir: head = NULL lalu tail diset NULL (tidak pernah dangling).
- ClearEvent: traverse seluruh node, free satu per satu, lalu head/tail/count
  di-zero. Aman dipanggil pada queue kosong.

## Pemeriksaan Memori

1. Pastikan setiap node hasil `malloc()` selalu di-`free()` persis satu kali
   (di PopEvent atau ClearEvent).
2. `PopEvent` menyimpan `next` SEBELUM `free(node)` agar tidak baca-after-free.
3. Jalankan dengan AddressSanitizer (lihat di atas) untuk deteksi
   use-after-free / double-free / leak.
4. Bila valgrind tersedia:

   ```bash
   valgrind --leak-check=full --error-exitcode=1 ./event_queue
   ```

   Keluaran `HEAP SUMMARY: in use at exit: 0 bytes` menandakan tidak ada leak.

## Asumsi (tanda tangan spesifikasi tidak terbaca penuh)

- Ukuran/tipe `Event` didefinisikan sebagai `{ int id; char name[64]; }`.
- `PushEvent` menyimpan **salinan** event (bukan pointer eksternal), sehingga
  pemanggil bebas memakai ulang buffer setelah push.
- `PopEvent` menyalin event keluar lewat `Event *out` dan return 0 saat kosong.
- Konvensi return 1/0 dipakai karena tidak ada detail boolean eksplisit di
  spesifikasi; kontrak API dapat disesuaikan 1 baris bila perlu.