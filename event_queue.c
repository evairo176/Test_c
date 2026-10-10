/*
 * event_queue.c - Implementasi Event Queue (Singly Linked List, FIFO).
 *
 * Desain:
 *   - Queue menyimpan pointer head (depan) dan tail (belakang).
 *   - PushEvent menyisipkan node baru di tail: O(1).
 *   - PopEvent menghapus node di head: O(1).
 *   - count dipertahankan agar GetEventCount O(1).
 */

#include "event_queue.h"
#include <stdlib.h>
#include <string.h>

int PushEvent(EventQueue *queue, const Event *event)
{
    if (queue == NULL || event == NULL)
        return 0;

    /* Alokasi node baru; gagal -> tidak ada perubahan pada queue. */
    EventNode *node = (EventNode *)malloc(sizeof(EventNode));
    if (node == NULL)
        return 0;

    node->event = *event; /* salin seluruh struct Event */
    node->next = NULL;

    if (queue->tail != NULL)
    {
        queue->tail->next = node;
        queue->tail = node;
    }
    else
    {
        /* Queue kosong: node ini jadi head sekaligus tail. */
        queue->head = node;
        queue->tail = node;
    }

    queue->count++;
    return 1;
}

int PopEvent(EventQueue *queue, Event *out)
{
    if (queue == NULL || out == NULL)
        return 0;

    EventNode *node = queue->head;
    if (node == NULL)
        return 0; /* queue kosong */

    *out = node->event;

    queue->head = node->next;
    if (queue->head == NULL)
        queue->tail = NULL; /* queue jadi kosong: tail tidak boleh dangling */

    queue->count--;
    free(node);
    return 1;
}

size_t GetEventCount(const EventQueue *queue)
{
    if (queue == NULL)
        return 0;
    return queue->count;
}

void ClearEvent(EventQueue *queue)
{
    if (queue == NULL)
        return;

    EventNode *cur = queue->head;
    while (cur != NULL)
    {
        EventNode *next = cur->next;
        free(cur);
        cur = next;
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
}