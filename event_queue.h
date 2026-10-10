/*
 * event_queue.h - Event Queue FIFO berbasis Singly Linked List.
 *
 * Kontrak API mengikuti spesifikasi ThreeSevenLab:
 *   PushEvent    - menambahkan event ke belakang queue (tail).
 *   PopEvent     - mengambil & menghapus event paling depan (head).
 *   GetEventCount- jumlah event yang tersimpan.
 *   ClearEvent   - menghapus semua event + membebaskan memori.
 *
 * Asumsi (tanda tangan tidak terbaca penuh di spesifikasi):
 *   - Event direpresentasikan sebagai struct Event { int id; char name[64]; }.
 *   - PushEvent menerima pointer ke Event (salinan dibuat di node baru).
 *   - PopEvent mengembalikan 1 jika berhasil (salinan ditaruh di *out),
 *     0 jika queue kosong.
 */
#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <stddef.h>

/* Panjang maksimum nama event (termasuk null terminator). */
#define EVENT_NAME_MAX 64

/* Representasi satu event. */
typedef struct Event
{
    int id;
    char name[EVENT_NAME_MAX];
} Event;

/* Node singly linked list. */
typedef struct EventNode
{
    Event event;
    struct EventNode *next;
} EventNode;

/* Queue: head = depan (paling lama), tail = belakang (paling baru). */
typedef struct EventQueue
{
    EventNode *head;
    EventNode *tail;
    size_t count;
} EventQueue;

/*
 * PushEvent: tambahkan salinan `event` ke belakang queue.
 * Return 1 sukses, 0 gagal (event NULL atau malloc gagal).
 */
int PushEvent(EventQueue *queue, const Event *event);

/*
 * PopEvent: ambil event paling depan, hapus dari queue, salin ke *out.
 * Return 1 sukses, 0 kalau queue kosong atau argumen tidak valid.
 */
int PopEvent(EventQueue *queue, Event *out);

/* GetEventCount: jumlah event yang sedang tersimpan. */
size_t GetEventCount(const EventQueue *queue);

/*
 * ClearEvent: hapus semua node dan free semua memori.
 * Aman dipanggil pada queue kosong atau queue yang belum di-inisialisasi
 * selama struct-nya di-zero (head/tail NULL, count 0).
 */
void ClearEvent(EventQueue *queue);

#endif /* EVENT_QUEUE_H */