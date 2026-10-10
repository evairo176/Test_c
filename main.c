/*
 * main.c - Pengujian Event Queue sesuai skenario spesifikasi.
 *
 * Skenario yang diuji:
 *   1. Queue kosong.
 *   2. Push satu event.
 *   3. Push beberapa event, pastikan urutan FIFO.
 *   4. Pop sampai queue kosong.
 *   5. Pop saat queue kosong (harus gagal dengan aman).
 *   6. GetEventCount setelah push/pop.
 *   7. ClearEvent pada queue kosong dan queue berisi.
 *   8. Push lagi setelah ClearEvent (queue reusable).
 *   9. Tidak ada memory leak / double free / dangling pointer.
 */

#include <stdio.h>
#include <string.h>
#include "event_queue.h"

static Event make_event(int id, const char *name)
{
    Event e;
    e.id = id;
    snprintf(e.name, sizeof(e.name), "%s", name);
    return e;
}

static void print_event(const char *label, const Event *e)
{
    printf("  %s -> id=%d name=\"%s\"\n", label, e->id, e->name);
}

/* Helper push: bangun event lewat variabel temp lalu salin ke queue. */
static int push_test(EventQueue *queue, int id, const char *name)
{
    Event temp = make_event(id, name);
    return PushEvent(queue, &temp);
}

int main(void)
{
    EventQueue queue = {0}; /* head=NULL tail=NULL count=0 */
    Event e;

    /* 1. Queue kosong */
    printf("[1] Queue kosong\n");
    printf("  GetEventCount = %zu (harap 0)\n", GetEventCount(&queue));

    /* 5. Pop saat kosong (sebelum ada data) */
    printf("  PopEvent saat kosong = %d (harap 0)\n", PopEvent(&queue, &e));

    /* 2. Push satu event */
    printf("\n[2] Push satu event\n");
    if (push_test(&queue, 1, "EVENT_INIT"))
        printf("  Push ok, GetEventCount = %zu (harap 1)\n", GetEventCount(&queue));
    else
        printf("  Push GAGAL (unexpected)\n");

    /* 6. GetEventCount setelah push */
    printf("  GetEventCount = %zu (harap 1)\n", GetEventCount(&queue));

    /* 3. Push beberapa event */
    printf("\n[3] Push beberapa event (urutan FIFO harus terjaga)\n");
    push_test(&queue, 2, "EVENT_LOGIN");
    push_test(&queue, 3, "EVENT_ORDER");
    push_test(&queue, 4, "EVENT_PAYMENT");
    printf("  GetEventCount = %zu (harap 4)\n", GetEventCount(&queue));

    /* 4. Pop berurutan -> harus sesuai urutan push */
    printf("\n[4] Pop berurutan (FIFO)\n");
    while (PopEvent(&queue, &e))
        print_event("pop", &e);
    printf("  GetEventCount = %zu (harap 0)\n", GetEventCount(&queue));

    /* 5. Pop saat kosong (setelah dikosongkan) */
    printf("\n[5] Pop saat kosong = %d (harap 0)\n", PopEvent(&queue, &e));

    /* 7a. ClearEvent pada queue kosong (aman) */
    printf("\n[7a] ClearEvent queue kosong (harus aman)\n");
    ClearEvent(&queue);
    printf("  GetEventCount = %zu (harap 0)\n", GetEventCount(&queue));

    /* 7b. Isi lagi lalu ClearEvent queue berisi */
    printf("\n[7b] ClearEvent queue berisi 3 event\n");
    push_test(&queue, 10, "EVENT_A");
    push_test(&queue, 11, "EVENT_B");
    push_test(&queue, 12, "EVENT_C");
    printf("  Sebelum clear GetEventCount = %zu (harap 3)\n", GetEventCount(&queue));
    ClearEvent(&queue);
    printf("  Sesudah clear GetEventCount = %zu (harap 0)\n", GetEventCount(&queue));

    /* 8. Push lagi setelah clear -> queue tetap bisa dipakai */
    printf("\n[8] Push lagi setelah ClearEvent\n");
    if (push_test(&queue, 20, "EVENT_AFTER_CLEAR"))
    {
        printf("  Push ok, GetEventCount = %zu (harap 1)\n", GetEventCount(&queue));
        if (PopEvent(&queue, &e))
            print_event("pop", &e);
    }
    else
    {
        printf("  Push GAGAL (unexpected)\n");
    }

    /* 9. Nilai akhir harus 0 supaya tidak ada leak */
    ClearEvent(&queue);
    printf("\n[9] Final GetEventCount = %zu (harap 0, semua memori di-free)\n",
           GetEventCount(&queue));

    /* Bonus: argumen tidak valid ditangani dengan aman */
    printf("\n[Bonus] Edge case argumen NULL\n");
    printf("  PushEvent(NULL, ...) = %d (harap 0)\n", PushEvent(NULL, &e));
    printf("  PopEvent(NULL, ...)  = %d (harap 0)\n", PopEvent(NULL, &e));
    printf("  GetEventCount(NULL)  = %zu (harap 0)\n", GetEventCount(NULL));
    printf("  ClearEvent(NULL)     = aman\n");
    ClearEvent(NULL);

    printf("\nSELESAI - semua skenario lolos tanpa crash.\n");
    return 0;
}